// Scroll timeline:
//  0%: "2019" made of floating trash
//  45%: "2019" scattered, ocean filling with plastic
//  98%: trash sinks, message assembles

const canvas = document.getElementById('oceanCanvas');
const ctx = canvas.getContext('2d');

// Canvas size
let W = canvas.width  = window.innerWidth;
let H = canvas.height = window.innerHeight;

window.addEventListener('resize', () => {
    W = canvas.width = window.innerWidth;
    H = canvas.height = window.innerHeight;
    gradientCache = null; 
    createPieces();
    create2019Text();
    createMessageText();
});


// Load total tons from data.json
let totalTons = 0; 

fetch('data.json')
    .then(r => r.json())
    .then(data => {
        for (const country of data.countries) totalTons += country.tons;
    })
    .catch(() => console.warn('Could not load data.json'));

// Returns 0 (top) to 1 (bottom) scroll progress
function getScroll() {
    const pageHeight = document.body.scrollHeight;
    const windowHeight = window.innerHeight;
    const maxScroll = pageHeight - windowHeight;
    if (maxScroll <= 0) return 0;
    return Math.min(1, window.scrollY / maxScroll);
}

// Color helpers
function mix(a, b, t) {
    return Math.round(a + (b - a) * t);
}

// Blend two RGB colors into a CSS string
function mixColor(r1,g1,b1,  r2,g2,b2,  t) {
    return `rgb(${mix(r1,r2,t)}, ${mix(g1,g2,t)}, ${mix(b1,b2,t)})`;
}


// Background darkens from bright to dark blue on scroll
let gradientCache     = null;
let gradientScrollBand = -1; // last scroll value, avoids rebuilding every frame

function drawBackground(scroll) {
    // Rebuild gradient only when scroll changes noticeably
    const band = Math.round(scroll * 700);
    if (band !== gradientScrollBand) {
        gradientScrollBand = band;

        gradientCache = ctx.createLinearGradient(0, 0, 0, H);
        // Top: bright blue
        gradientCache.addColorStop(0, mixColor(0,207,255,  13,37,53,  scroll));
        // Bottom: dark blue
        gradientCache.addColorStop(1, mixColor(0, 92,128,   8,24,32,  scroll));
    }

    ctx.setTransform(1, 0, 0, 1, 0, 0);
    ctx.globalAlpha = 1;
    ctx.fillStyle   = gradientCache;
    ctx.fillRect(0, 0, W, H);
}

// Plastic piece colors
let lastColorScroll = -1;
let pieceColor = 'rgb(218,238,255)';
let glowColor  = 'rgb(168,216,255)';

function updatePieceColors(scroll) {
    // Only recalc when scroll changes enough
    if (Math.abs(scroll - lastColorScroll) < 0.005) return;
    lastColorScroll = scroll;

    // Pale blue/white → grey
    pieceColor = mixColor(218,238,255,  122,140,145,  scroll);
    // Bright glow → dull glow
    glowColor  = mixColor(168,216,255,   58, 74, 80,  scroll);
}

// Shape drawers. Each draws at (0,0). Position set via ctx.setTransform()
const SHAPES = [
    // Rectangle = bottle caps / packaging
    (size, sx, sy) =>
        ctx.fillRect(-size*sx*0.5, -size*sy*0.5, size*sx, size*sy),

    // Ellipse = plastic bags
    (size, sx, sy) => {
        ctx.beginPath();
        ctx.ellipse(0, 0, size*sx*0.55, size*sy*0.3, 0, 0, Math.PI*2);
        ctx.fill();
    },

    // Triangle = broken plastic
    // bp[] = pre-baked random offsets for variation
    (size, sx, sy, bp) => {
        ctx.beginPath();
        ctx.moveTo(0, -size*(0.5  + bp[0]*0.3));
        ctx.lineTo( size*(0.45 + bp[1]*0.25), size*(0.35 + bp[2]*0.2));
        ctx.lineTo(-size*(0.4  + bp[3]*0.25), size*(0.3  + bp[4]*0.2));
        ctx.closePath();
        ctx.fill();
    },

    // Thin line = straws / fishing line
    (size, sx) => {
        ctx.lineWidth = 3;
        ctx.beginPath();
        ctx.moveTo(-size*sx*0.7, -size*0.08);
        ctx.lineTo( size*sx*0.7,  size*0.08);
        ctx.stroke();
    },

    // Blob polygon = crumpled pieces
    (size, sx, sy, bp) => {
        ctx.beginPath();
        for (let i = 0; i < 5; i++) {
            const angle  = (i / 5) * Math.PI * 2;
            const radius = size * (0.35 + bp[i % 6] * 0.3);
            const px = Math.cos(angle) * radius * sx;
            const py = Math.sin(angle) * radius * sy;
            i === 0 ? ctx.moveTo(px, py) : ctx.lineTo(px, py);
        }
        ctx.closePath();
        ctx.fill();
    },

    // Ring = bottle tops / plastic rings
    (size, sx) => {
        ctx.lineWidth = size * 0.15;
        ctx.beginPath();
        ctx.arc(0, 0, size*0.3*sx, 0, Math.PI*2);
        ctx.stroke();
    },
];

// Draw a shape at (x, y)
function drawShapeAt(piece, x, y, alpha, fillColor, strokeColor) {
    ctx.globalAlpha = alpha;
    ctx.fillStyle = fillColor || pieceColor;
    ctx.strokeStyle = strokeColor || glowColor;
    ctx.setTransform(piece.ax, 0, 0, piece.ay, x, y);
    ctx.rotate(piece.rot);
    SHAPES[piece.shape](piece.size, piece.ax, piece.ay, piece.bp);
}

// Sinking helpers
// 0 = not sinking, 1 = fully sunk
// sinkDelay staggers pieces so they fall one by one
function getSinkProgress(globalSink, sinkDelay) {
    return Math.max(0, (globalSink - sinkDelay) / (1 - sinkDelay + 0.001));
}

// Fall curve: fast start, slows as piece settles
function getFallAmount(progress) {
    if (progress < 0.7) {
        return Math.pow(progress / 0.7, 1.8) * 0.85;
    }

    return 0.85 + ((progress - 0.7) / 0.3) * 0.15;
}

// Edge pieces pile lower
function calcRestY(piece) {
    const distFromCentre = Math.abs(piece.restX - W * 0.5) / (W * 0.5);
    return H - piece.size * (0.5 + piece.wAmp * 0.2) - distFromCentre * H * 0.04;
}

// Floating plastic pieces
const MAX_PIECES = 1000;
const pieces     = [];

// Generate random plastic piece properties
function makePiece() {
    return {
        // Random start position, upper screen
        x: ((Math.random() * W) + (Math.random() - 0.5) * W * 0.2 + W) % W,
        y: H * 0.05 + Math.pow(Math.random(), 0.7) * H * 0.88,

        size: 3 + Math.pow(Math.random(), 1.8) * 32, // mostly small
        ax: 0.3 + Math.random() * 1.4,   // x stretch
        ay: 0.3 + Math.random() * 1.4,   // y stretch
        rot: Math.random() * Math.PI * 2,  // start angle
        rotSpd: (Math.random() < 0.15 ? 1 : 0.08) * (Math.random() - 0.5) * 0.06,
        opacity: 0.3 + Math.random() * 0.65,
        dx: (Math.random() - 0.5) * 0.22, // horizontal speed
        dy: (Math.random() - 0.5) * 0.05, // vertical speed
        wAmp: Math.random() * 6,             // bob height
        wFreq: 0.0008 + Math.random() * 0.003,// bob speed
        wOff: Math.random() * Math.PI * 2,   // timing offset
        shape: Math.floor(Math.random() * 6),
        bp: Array.from({length: 6}, () => (Math.random() - 0.5) * 0.7),

        sinkDelay: Math.random(),       // sink delay (0–1)
        floater: Math.random() < 0.3, // 30% never sink
        sinkY: null,                // y when sinking started
        restX: Math.random() * W,   // x in final pile
        restY: null,
        sunk: false,               // true once at bottom
    };
}

// Create plastic pieces
function createPieces() {
    pieces.length = 0;
    for (let i = 0; i < MAX_PIECES; i++) {
        pieces.push(makePiece());
    }
}
createPieces();

function drawPiece(p, time, globalSink) {
    let offsetX = 0, offsetY = 0, extraSpin = 0;

    // Sinking offset if this piece should fall
    if (globalSink > 0 && !p.floater) {
        const sinkProgress = getSinkProgress(globalSink, p.sinkDelay);

        if (sinkProgress > 0) {
            // Record start of fall for offset calc
            if (p.sinkY === null) {
                p.sinkY = p.y;
                p.restY = calcRestY(p);
            }

            const fall = getFallAmount(sinkProgress);
            offsetX = (p.restX - p.x) * fall;
            offsetY = (p.restY - p.sinkY) * fall;
            extraSpin = sinkProgress * Math.PI * (2 + p.sinkDelay * 3) * (1 - fall * 0.9);

            if (sinkProgress >= 0.999) {
                p.sunk = true;
            }
        }
    }

    // Spin a bit each frame
    p.rot += p.rotSpd; 

    // Frozen at bottom once sunk
    if (p.sunk) {
        ctx.setTransform(1, 0, 0, 1, p.restX, p.restY);
        ctx.rotate(p.rot);
        ctx.globalAlpha = p.opacity * 0.9;
        ctx.fillStyle = pieceColor;
        ctx.strokeStyle = glowColor;
        SHAPES[p.shape](p.size, p.ax, p.ay, p.bp);
        return;
    }

    // Floating: drift, wrap horizontally, clamp vertically
    p.x = (p.x + p.dx + W) % W;
    p.y = Math.max(H*0.02, Math.min(H*0.97, p.y + p.dy));
    // Bounce at top/bottom
    if (p.y >= H*0.97 || p.y <= H*0.02) p.dy *= -1;

    const bob = Math.sin(time * p.wFreq + p.wOff) * p.wAmp;   // gentle bob
    const pulse  = 0.85 + 0.15 * Math.sin(time * 0.0018 + p.x * 0.01); // opacity shimmer

    ctx.setTransform(1, 0, 0, 1, p.x + offsetX, p.y + bob + offsetY);
    ctx.rotate(p.rot + extraSpin);
    ctx.globalAlpha = p.opacity * pulse;
    ctx.fillStyle   = pieceColor;
    ctx.strokeStyle = glowColor;
    SHAPES[p.shape](p.size, p.ax, p.ay, p.bp);
}


// Build text from particles
function sampleTextPixels(text, font, maxParticles, pixelStep, minAlpha) {
    // Hidden canvas for text measurement
    const offscreen = document.createElement('canvas');
    offscreen.width  = W;
    offscreen.height = Math.ceil(H * 0.4);

    const offCtx = offscreen.getContext('2d');
    offCtx.font          = font;
    offCtx.fillStyle     = '#fff';
    offCtx.textBaseline  = 'top';

    // Center text horizontally
    const textWidth = offCtx.measureText(text).width;
    offCtx.fillText(text, (W - textWidth) / 2, 0);

    // Read pixels and collect filled ones
    const pixels       = offCtx.getImageData(0, 0, offscreen.width, offscreen.height).data;
    const filledPixels = [];

    for (let y = 0; y < offscreen.height; y += pixelStep) {
        for (let x = 0; x < offscreen.width; x += pixelStep) {
            const alphaChannel = pixels[(y * offscreen.width + x) * 4 + 3]; // 0=transparent, 255=opaque
            if (alphaChannel > minAlpha) filledPixels.push([x, y]);
        }
    }

    // Shuffle for random distribution across letters
    for (let i = filledPixels.length - 1; i > 0; i--) {
        const j = Math.floor(Math.random() * (i + 1));
        [filledPixels[i], filledPixels[j]] = [filledPixels[j], filledPixels[i]];
    }

    return filledPixels.slice(0, maxParticles);
}

// Create a particle at (x, y)
// Shared by 2019 text and end message
function makeParticle(x, y, shapeCount, minSize, maxSizeRange, rotSpeed) {
    return {
        x, y,
        originX: x, originY: y, // start position for sinking
        size:    minSize + Math.random() * maxSizeRange,
        rot:     Math.random() * Math.PI * 2,
        rotSpd:  (Math.random() - 0.5) * rotSpeed,
        shape:   Math.floor(Math.random() * shapeCount),
        ax:      0.4 + Math.random() * 1.2,
        ay:      0.4 + Math.random() * 1.2,
        bp:      Array.from({length: 6}, () => (Math.random() - 0.5) * 0.7),
        wAmp:    0.5 + Math.random() * 2,
        wFreq:   0.001 + Math.random() * 0.003,
        wOff:    Math.random() * Math.PI * 2,
    };
}



// "2019" trash text that is visible at start, scatters on scroll
let yearParticles = [];
let yearReady     = false;

function create2019Text() {
    const fontSize     = Math.min(W * 0.38, 300);
    const textHeight   = Math.round(fontSize * 1.3);
    const verticalPos  = (H - textHeight) * 0.42; // slightly above center

    const pixelPositions = sampleTextPixels('2019', `900 ${fontSize}px Arial, sans-serif`, 700, 3, 128);

    yearParticles = pixelPositions.map(([px, py]) => ({
        ...makeParticle(
            px + (Math.random() - 0.5) * 4,
            py + verticalPos + (Math.random() - 0.5) * 4,
            4, 1.5, 5, 0.04
        ),
        scatterDirX: (Math.random() - 0.5) * 2, // scatter direction
        scatterDirY: (Math.random() - 0.5) * 2,
        sinkDelay:   Math.random(),
        restX:       20 + Math.random() * (W - 40),
        restY:       null,
        spinDir:     Math.random() < 0.5 ? 1 : -1,
    }));

    yearReady = true;
}
create2019Text();

function draw2019Text(scroll, time, globalSink) {
    // Fade out by 45% scroll
    const fadeProgress = Math.min(1, scroll / 0.45);
    const alpha        = (1 - fadeProgress) * 0.55;
    if (alpha <= 0 || !yearReady) return;

    // Pale blue → grey as particles scatter
    const color = mixColor(218,238,255,  122,140,145,  Math.min(1, scroll / 0.6));

    ctx.setTransform(1, 0, 0, 1, 0, 0);

    for (const p of yearParticles) {
        // Quadratic scatter and accelerates as they fade
        const scatter = fadeProgress * fadeProgress;
        let shiftX = p.scatterDirX * scatter * W * 0.35;
        let shiftY = p.scatterDirY * scatter * H * 0.35;
        let spin   = 0;

        // Sink at bottom (same logic as regular pieces)
        if (globalSink > 0) {
            const sp = getSinkProgress(globalSink, p.sinkDelay);
            if (sp > 0) {
                if (!p.restY) p.restY = H - p.size * 1.5 - Math.abs(p.restX - W*0.5) / (W*0.5) * H * 0.04;
                const fall = getFallAmount(sp);
                shiftX += (p.restX - p.originX) * fall;
                shiftY += (p.restY - p.originY) * fall;
                spin    = sp * Math.PI * (2 + p.sinkDelay * 3) * (1 - fall * 0.9) * p.spinDir;
            }
        }

        const bob = globalSink > 0 ? 0 : Math.sin(time * p.wFreq + p.wOff) * p.wAmp;
        p.rot += p.rotSpd;

        ctx.globalAlpha = alpha;
        ctx.fillStyle   = color;
        ctx.strokeStyle = color;
        ctx.setTransform(p.ax, 0, 0, p.ay, p.x + shiftX, p.y + bob + shiftY);
        ctx.rotate(p.rot + spin);
        SHAPES[p.shape](p.size, p.ax, p.ay, p.bp);
    }

    ctx.globalAlpha = 1;
}


// end message that assembles from particles when trash sinks
let messageParticles = [];
let messageReady     = false;

function createMessageText() {
    messageParticles = [];

    const lines    = ["The ocean doesn't forget", "about what we throw away."];
    const fontSize = Math.max(32, Math.min(W * 0.085, 82));
    const lineH    = fontSize * 1.45;
    const startY   = H * 0.5 - lineH; // center both lines

    lines.forEach((line, lineIndex) => {
        const positions = sampleTextPixels(line, `900 ${fontSize}px Arial, sans-serif`, 4000, 1, 60);
        const lineY     = startY + lineIndex * lineH;

        for (const [px, py] of positions) {
            messageParticles.push({
                ...makeParticle(
                    px + (Math.random() - 0.5) * 1,
                    lineY + py + (Math.random() - 0.5) * 1,
                    4, 0.8, 1.2, 0.008
                ),
                ax:       0.9 + Math.random() * 0.2,
                ay:       0.9 + Math.random() * 0.2,
                // Stagger reveal so text assembles gradually
                revealAt: 0.55 + Math.random() * 0.45,
            });
        }
    });

    messageReady = true;
}
createMessageText();

function drawMessageText(globalSink, time) {
    if (!messageReady || globalSink < 0.55) return;

    // Fade in over sinkProgress 0.55 → 0.75
    const overallAlpha = Math.min(1, (globalSink - 0.55) / 0.20);
    if (overallAlpha <= 0) return;

    ctx.setTransform(1, 0, 0, 1, 0, 0);

    for (const p of messageParticles) {
        // Fade in once globalSink passes this particle's revealAt
        const alpha = overallAlpha * Math.min(1, Math.max(0, (globalSink - p.revealAt) / 0.05));
        if (alpha < 0.02) continue;

        const bob = Math.sin(time * p.wFreq + p.wOff) * p.wAmp;
        p.rot += p.rotSpd;

        ctx.globalAlpha = alpha;
        ctx.fillStyle   = '#eef8ff'; // pale icy white
        ctx.strokeStyle = '#c8e8f5';
        ctx.setTransform(p.ax, 0, 0, p.ay, p.x, p.y + bob);
        ctx.rotate(p.rot);
        SHAPES[p.shape](p.size, p.ax, p.ay, p.bp);
    }

    ctx.globalAlpha = 1;
}


// sink progress. Rises when user reaches page bottom
let globalSink = 0;

function updateSink(scroll) {
    const target = scroll >= 0.98 ? 1 : 0; // 1 = at bottom, 0 = not
    globalSink  += (target - globalSink) * 0.004; // ease toward target (lerp)

    // Un-sink if user scrolls back up
    if (globalSink < 0.01) {
        for (const p of pieces) {
            p.sunk  = false;
            p.sinkY = null;
        }
    }

    return globalSink;
}

// Cache instruction element to avoid per-frame lookup
const instructionEl = document.querySelector('.instruction');

function draw() {
    const scroll = getScroll();
    const time   = Date.now();  // ms since load, for animation timing
    const sink   = updateSink(scroll);

    // Update "X tons" counter
    document.getElementById('massLabel').innerText =
        Math.round(scroll * totalTons).toLocaleString() + ' tons';

    // Fade "↓ scroll down" hint out by 15% scroll
    if (instructionEl) instructionEl.style.opacity = Math.max(0, 1 - scroll / 0.15);

    // Draw back-to-front (painter's algorithm)
    updatePieceColors(scroll);
    drawBackground(scroll);

    ctx.shadowBlur = 0; // shadows are expensive

    // Square-root curve so ocean fills fast at first
    const numVisible = Math.floor(Math.sqrt(scroll) * MAX_PIECES);
    for (let i = 0; i < numVisible; i++) drawPiece(pieces[i], time, sink);

    // Reset transform after pieces
    ctx.setTransform(1, 0, 0, 1, 0, 0);
    ctx.globalAlpha = 1;

    draw2019Text(scroll, time, sink);
    drawMessageText(sink, time);

    requestAnimationFrame(draw); // next frame
}

// Make page tall enough to scroll (4 screen heights)
document.body.style.minHeight = '400vh';

// Hide HTML year label. "2019" is drawn on canvas
document.getElementById('yearLabel').style.display = 'none';

// Start animation
requestAnimationFrame(draw);