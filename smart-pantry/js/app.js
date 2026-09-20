// Check that user is logged in
const userId = localStorage.getItem('user_id');
const username = localStorage.getItem('username');

if (!userId) {
    window.location.href = 'login.html';
}

// Show welcome text in the top nav
document.getElementById('welcomeText').textContent = `Hi, ${username}!`;

// Logout button
document.getElementById('logoutBtn').addEventListener('click', () => {
    localStorage.clear();
    window.location.href = 'login.html';
});


// Tab switching
const tabButtons = document.querySelectorAll('.tab-btn');
const tabContents = document.querySelectorAll('.tab-content');

tabButtons.forEach(btn => {
    btn.addEventListener('click', () => {
        // Get which tab was clicked
        const tabName = btn.getAttribute('data-tab');

        // Remove active class
        tabButtons.forEach(b => b.classList.remove('active'));
        tabContents.forEach(c => c.classList.remove('active'));

        // Add active class to the clicked tab
        btn.classList.add('active');
        document.getElementById(`${tabName}-tab`).classList.add('active');

        // Hide the expiration alert banner on tabs where it isn't relevant
        const banner = document.getElementById('alertBanner');
        if (tabName !== 'pantry') {
            banner.classList.add('hidden');
        }

        // Reload tab's data when the Tab is switched
        if (tabName === 'pantry') loadPantry();
        if (tabName === 'recipes') loadRecipes();
        if (tabName === 'shopping') loadShoppingList();
        if (tabName === 'saved') loadSaved();
        if (tabName === 'templates') loadTemplates();
        if (tabName === 'history') loadHistory();
    });
});

// Pantry tab
async function loadPantry() {
    const list = document.getElementById('pantryList');
    list.innerHTML = '<p class="empty-state">Loading...</p>';

    try {
        // Fetch pantry items from the server
        const res = await fetch(`/api/pantry/${userId}`);
        const items = await res.json();

        // Show a message if empty
        if (items.length === 0) {
            list.innerHTML = '<p class="empty-state">Your pantry is empty. Add some items or use a template!</p>';
            document.getElementById('alertBanner').classList.add('hidden');
            return;
        }

        // Check for items expiring soon (within 3 days) or expired
        const expiringSoon = [];
        const expired = [];
        const today = new Date();
        today.setHours(0, 0, 0, 0);

        items.forEach(item => {
            if (!item.expiration_date) return;
            const expDate = new Date(item.expiration_date);
            const daysDiff = Math.ceil((expDate - today) / (1000 * 60 * 60 * 24));

            if (daysDiff < 0) {
                expired.push(item.name);
            } else if (daysDiff <= 3) {
                expiringSoon.push(item.name);
            }
        });

        // Show alert banner if there are items expiring soon
        const banner = document.getElementById('alertBanner');
        if (expired.length > 0 || expiringSoon.length > 0) {
            let msg = '';
            if (expired.length > 0) {
                msg += `${expired.length} item(s) expired: ${expired.join(', ')}. `;
            }
            if (expiringSoon.length > 0) {
                msg += `${expiringSoon.length} item(s) expiring soon: ${expiringSoon.join(', ')}.`;
            }
            banner.textContent = msg;
            banner.classList.remove('hidden');
        } else {
            banner.classList.add('hidden');
        }

        // Build HTML for each pantry item
        list.innerHTML = '';
        items.forEach(item => {
            const itemDiv = document.createElement('div');
            itemDiv.className = 'pantry-item';

            // Calculate how close to expiration
            let expTag = '';
            let expClass = '';
            if (item.expiration_date) {
                const expDate = new Date(item.expiration_date);
                const daysDiff = Math.ceil((expDate - today) / (1000 * 60 * 60 * 24));
                const expFormatted = expDate.toLocaleDateString();

                if (daysDiff < 0) {
                    expClass = 'expired';
                    expTag = `<span class="exp-tag danger">Expired ${expFormatted}</span>`;
                } else if (daysDiff <= 3) {
                    expClass = 'expiring-soon';
                    expTag = `<span class="exp-tag warning">Use by ${expFormatted} (${daysDiff}d)</span>`;
                } else {
                    expTag = `<span class="exp-tag safe">Expires ${expFormatted}</span>`;
                }
            }

            if (expClass) itemDiv.classList.add(expClass);

            // Build the item HTML
            itemDiv.innerHTML = `
                <div class="item-info">
                    <div class="item-name">${escapeHTML(item.name)}</div>
                    <div class="item-details">
                        ${item.quantity} ${escapeHTML(item.unit)}
                    </div>
                    ${expTag}
                </div>
                <div class="item-actions">
                    <button class="btn-small" onclick="editItem('${item.item_id}', '${escapeForJS(item.name)}', ${item.quantity}, '${escapeForJS(item.unit)}', '${item.expiration_date || ''}')">Edit</button>
                    <button class="btn-danger" onclick="deleteItem('${item.item_id}')">Delete</button>
                </div>
            `;
            list.appendChild(itemDiv);
        });
    } catch (err) {
        list.innerHTML = '<p class="empty-state">Could not load pantry items.</p>';
        console.log('Load pantry error:', err);
    }
}

// --- Helper: escape HTML to prevent XSS ---------------------
function escapeHTML(str) {
    if (!str) return '';
    return String(str)
        .replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;')
        .replace(/"/g, '&quot;')
        .replace(/'/g, '&#39;');
}

// --- Helper: escape for JS single-quoted inline handlers ---
function escapeForJS(str) {
    if (!str) return '';
    return String(str).replace(/'/g, "\\'");
}

// "Add to Pantry" form submission
document.getElementById('addItemForm').addEventListener('submit', async (e) => {
    e.preventDefault();

    // Collect form values
    const name = document.getElementById('itemName').value;
    const quantity = document.getElementById('itemQty').value;
    const unit = document.getElementById('itemUnit').value;
    const expiration_date = document.getElementById('itemExp').value;

    try {
        // Send to server
        const res = await fetch('/api/pantry', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                user_id: userId,
                name,
                quantity,
                unit,
                expiration_date: expiration_date || null
            })
        });

        if (res.ok) {
            // Clear the form and reload the list
            document.getElementById('addItemForm').reset();
            document.getElementById('itemQty').value = 1;
            document.getElementById('foodInfoResults').innerHTML = '';
            loadPantry();
        } else {
            alert('Could not add item.');
        }
    } catch (err) {
        alert('Server error. Is the server running?');
    }
});

// Delete an item
async function deleteItem(itemId) {
    // Confirm before deleting
    if (!confirm('Delete this item from your pantry?')) return;

    try {
        const res = await fetch(`/api/pantry/${itemId}`, { method: 'DELETE' });
        if (res.ok) loadPantry();
    } catch (err) {
        alert('Could not delete item.');
    }
}

// Edit an item
async function editItem(itemId, name, quantity, unit, expDate) {
    // Ask user for new values
    const newName = prompt('Item name:', name);
    if (newName === null) return;

    const newQty = prompt('Quantity:', quantity);
    if (newQty === null) return;

    const newUnit = prompt('Unit:', unit);
    if (newUnit === null) return;

    const newExp = prompt('Expiration date (YYYY-MM-DD) or leave blank:', expDate ? expDate.split('T')[0] : '');
    if (newExp === null) return;

    try {
        const res = await fetch(`/api/pantry/${itemId}`, {
            method: 'PUT',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                name: newName,
                quantity: newQty,
                unit: newUnit,
                expiration_date: newExp || null
            })
        });
        if (res.ok) loadPantry();
    } catch (err) {
        alert('Could not update item.');
    }
}

// Food lookup button — calls USDA FoodData Central via /api/foodinfo
document.getElementById('lookupBtn').addEventListener('click', async () => {
    const name = document.getElementById('itemName').value.trim();
    const resultsDiv = document.getElementById('foodInfoResults');

    if (!name) {
        alert('Type an item name first to look it up.');
        return;
    }

    resultsDiv.innerHTML = '<p style="color: #666;">Searching...</p>';

    try {
        const res = await fetch(`/api/foodinfo?query=${encodeURIComponent(name)}`);
        const data = await res.json();

        if (!res.ok) {
            resultsDiv.innerHTML = `<p style="color: #F44336;">${escapeHTML(data.error || 'Could not fetch food info.')}</p>`;
            return;
        }

        if (!data.length) {
            resultsDiv.innerHTML = '<p style="color: #666;">No results found.</p>';
            return;
        }

        resultsDiv.innerHTML = '<p style="color: #666; font-size: 13px; margin-bottom: 5px;">Nutrition info (USDA · per 100g):</p>';
        data.forEach(item => {
            const div = document.createElement('div');
            div.className = 'food-info-item';
            div.innerHTML = `
                <div class="food-details">
                    <strong>${escapeHTML(item.name)}</strong>
                    ${item.brand ? `<div style="color:#666; font-size:12px;">${escapeHTML(item.brand)}</div>` : ''}
                    <div class="nutrition-grid">
                        <span>${item.calories} cal</span>
                        <span>${item.protein_g}g protein</span>
                        <span>${item.fat_g}g fat</span>
                        <span>${item.carbs_g}g carbs</span>
                        <span>${item.sugar_g}g sugar</span>
                        <span>${item.sodium_mg}mg sodium</span>
                    </div>
                </div>
            `;
            resultsDiv.appendChild(div);
        });
    } catch (err) {
        resultsDiv.innerHTML = '<p style="color: #F44336;">Could not fetch food info.</p>';
    }
});


// Recipes tab — populate ingredient picker from pantry
async function loadRecipes() {
    const checkboxContainer = document.getElementById('ingredientCheckboxes');
    checkboxContainer.innerHTML = '<p class="empty-state">Loading your pantry...</p>';

    // Hide previous results when switching to this tab
    document.getElementById('recipeResults').classList.add('hidden');

    try {
        const res = await fetch(`/api/pantry/${userId}`);
        const items = await res.json();

        if (!items.length) {
            checkboxContainer.innerHTML = '<p class="empty-state">Add items to your pantry first.</p>';
            return;
        }

        // Deduplicate ingredient names (case-insensitive)
        const seen = new Set();
        const unique = items.filter(item => {
            const key = item.name.toLowerCase();
            if (seen.has(key)) return false;
            seen.add(key);
            return true;
        });

        checkboxContainer.innerHTML = '';
        unique.forEach(item => {
            const label = document.createElement('label');
            label.className = 'ingredient-checkbox-label';
            label.innerHTML = `
                <input type="checkbox" value="${escapeHTML(item.name)}" checked>
                ${escapeHTML(item.name)}
            `;
            checkboxContainer.appendChild(label);
        });
    } catch (err) {
        checkboxContainer.innerHTML = '<p class="empty-state">Could not load pantry.</p>';
    }
}

// Fetch and render recipes based on selected ingredients
async function findRecipes() {
    const checkboxes = document.querySelectorAll('#ingredientCheckboxes input[type="checkbox"]:checked');
    const selected = Array.from(checkboxes).map(cb => cb.value);

    const resultsDiv = document.getElementById('recipeResults');
    const list = document.getElementById('recipesList');

    if (!selected.length) {
        alert('Please select at least one ingredient.');
        return;
    }

    resultsDiv.classList.remove('hidden');
    list.innerHTML = '<p class="empty-state">Finding recipes for you...</p>';

    try {
        const params = encodeURIComponent(selected.join(','));
        const res = await fetch(`/api/recipes/recommendations/${userId}?ingredients=${params}`);
        const recipes = await res.json();

        if (!recipes.length) {
            list.innerHTML = '<p class="empty-state">No recipes found with those ingredients.</p>';
            return;
        }

        list.innerHTML = '';
        recipes.forEach(r => {
            const card = document.createElement('div');
            card.className = 'recipe-card';

            let matchClass = 'low';
            if (r.match_percent >= 80) matchClass = 'high';
            else if (r.match_percent >= 50) matchClass = 'medium';

            const priorityBadge = r.uses_expiring_count > 0
                ? `<div class="priority-badge">Uses expiring items!</div>`
                : '';

            card.innerHTML = `
                ${r.image ? `<img src="${r.image}" alt="${escapeHTML(r.name)}" class="recipe-thumb">` : ''}
                ${priorityBadge}
                <h4>${escapeHTML(r.name)}</h4>
                <p class="description">${escapeHTML(r.description || '')}</p>
                <span class="match-badge ${matchClass}">
                    ${r.match_percent}% match (${r.available_count}/${r.total_ingredients} ingredients)
                </span>
            `;

            card.addEventListener('click', () => openRecipeModal(r));
            list.appendChild(card);
        });
    } catch (err) {
        list.innerHTML = '<p class="empty-state">Could not load recipes.</p>';
    }
}

// Wire up ingredient picker buttons
document.getElementById('findRecipesBtn').addEventListener('click', findRecipes);

document.getElementById('selectAllBtn').addEventListener('click', () => {
    document.querySelectorAll('#ingredientCheckboxes input[type="checkbox"]').forEach(cb => cb.checked = true);
});

document.getElementById('clearAllBtn').addEventListener('click', () => {
    document.querySelectorAll('#ingredientCheckboxes input[type="checkbox"]').forEach(cb => cb.checked = false);
});

// Open modal with recipe details
function openRecipeModal(recipe) {
    const modal = document.getElementById('recipeModal');
    const body = document.getElementById('modalBody');

    // ingredients is now an array from MongoDB
    const ingredients = recipe.ingredients || [];
    const missing = recipe.missing_ingredients || [];

    // Build a list showing which ingredients user has and which are missing
    const ingredientList = ingredients.map(ing => {
        const isMissing = missing.some(m => m.toLowerCase() === ing.toLowerCase());
        const cls = isMissing ? 'missing' : 'have';
        return `<li class="${cls}">${escapeHTML(ing)}</li>`;
    }).join('');

    // If the recipe has missing ingredients, show an "Add to Shopping List" button
    // Encode the missing array as a JSON string so we can pass it through onclick
    const missingJson = encodeURIComponent(JSON.stringify(missing));
    const shoppingBtn = missing.length > 0
        ? `<button class="btn-secondary" onclick="addMissingToShoppingList('${missingJson}', '${escapeForJS(recipe.name)}')">Add Missing to Shopping List</button>`
        : '';

    body.innerHTML = `
        <h2>${escapeHTML(recipe.name)}</h2>
        ${recipe.image ? `<img src="${recipe.image}" alt="${escapeHTML(recipe.name)}" class="recipe-modal-img">` : ''}
        <p class="description" style="color:#666; margin-bottom:15px;">${escapeHTML(recipe.description || '')}</p>

        <h3>Ingredients</h3>
        <ul class="ingredient-list">${ingredientList}</ul>

        <h3>Instructions</h3>
        <div class="recipe-instructions">${escapeHTML(recipe.instructions)}</div>
        <p class="units-note">Note: measurements are sourced from TheMealDB and may mix metric and imperial units depending on the recipe.</p>

        <div class="modal-actions">
            <button class="btn-primary" id="cookBtn">Mark as Cooked</button>
            <button class="btn-secondary" id="saveBtn">Save Recipe</button>
            ${shoppingBtn}
        </div>
    `;

    document.getElementById('cookBtn').addEventListener('click', () => markAsCooked(recipe));
    document.getElementById('saveBtn').addEventListener('click', () => saveRecipe(recipe));
    modal.classList.remove('hidden');
}

// Close modal
document.getElementById('modalClose').addEventListener('click', () => {
    document.getElementById('recipeModal').classList.add('hidden');
});

// Click outside the modal to close it
document.getElementById('recipeModal').addEventListener('click', (e) => {
    if (e.target.id === 'recipeModal') {
        document.getElementById('recipeModal').classList.add('hidden');
    }
});


// Mark as cooked and sends full recipe data since TheMealDB recipes are not int the DB
async function markAsCooked(recipe) {
    if (!confirm('Mark this recipe as cooked? This will remove one unit of each ingredient from your pantry.')) return;

    try {
        const res = await fetch('/api/recipes/cook', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                user_id: userId,
                recipe_id: recipe.recipe_id,
                recipe_name: recipe.name,
                ingredients: recipe.ingredients
            })
        });
        if (res.ok) {
            alert('Pantry updated!');
            document.getElementById('recipeModal').classList.add('hidden');
            findRecipes();
        }
    } catch (err) {
        alert('Could not update pantry.');
    }
}

// Cooking history tab
async function loadHistory() {
    const list = document.getElementById('historyList');
    list.innerHTML = '<p class="empty-state">Loading history...</p>';

    try {
        const res = await fetch(`/api/history/${userId}`);
        const entries = await res.json();

        if (entries.length === 0) {
            list.innerHTML = '<p class="empty-state">No cooking history yet. Mark a recipe as cooked to get started!</p>';
            return;
        }

        list.innerHTML = entries.map(entry => {
            const date = new Date(entry.cooked_at).toLocaleDateString(undefined, {
                year: 'numeric', month: 'short', day: 'numeric'
            });
            return `<div class="recipe-card">
                <h3>${entry.recipe_name}</h3>
                <p class="help-text">Cooked on ${date}</p>
            </div>`;
        }).join('');
    } catch (err) {
        list.innerHTML = '<p class="empty-state">Could not load history.</p>';
    }
}

// Save recipe and stores full recipe data so you don't need to re-fetch from TheMealDB
async function saveRecipe(recipe) {
    try {
        const res = await fetch('/api/saved', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                user_id: userId,
                recipe_id: recipe.recipe_id,
                recipe_name: recipe.name,
                description: recipe.description,
                ingredients: recipe.ingredients,
                instructions: recipe.instructions,
                image: recipe.image || ''
            })
        });
        const data = await res.json();
        if (res.ok) {
            alert(data.message === 'Already saved' ? 'Already in your saved recipes.' : 'Recipe saved!');
        }
    } catch (err) {
        alert('Could not save recipe.');
    }
}


// Saved recipes tab
async function loadSaved() {
    const list = document.getElementById('savedList');
    list.innerHTML = '<p class="empty-state">Loading saved recipes...</p>';

    try {
        const res = await fetch(`/api/saved/${userId}`);
        const recipes = await res.json();

        if (recipes.length === 0) {
            list.innerHTML = '<p class="empty-state">No saved recipes yet. Save some from the Recipes tab!</p>';
            return;
        }

        list.innerHTML = '';
        recipes.forEach(r => {
            const card = document.createElement('div');
            card.className = 'recipe-card';
            card.innerHTML = `
                ${r.image ? `<img src="${r.image}" alt="${escapeHTML(r.name)}" class="recipe-thumb">` : ''}
                <h4>${escapeHTML(r.name)}</h4>
                <p class="description">${escapeHTML(r.description || '')}</p>
                <button class="btn-danger" onclick="event.stopPropagation(); unsaveRecipe('${r.saved_id}')" style="margin-top:10px;">Remove</button>
            `;
            card.addEventListener('click', () => {
                // Need missing_ingredients for the modal; compute empty list here
                openRecipeModal({ ...r, missing_ingredients: [] });
            });
            list.appendChild(card);
        });
    } catch (err) {
        list.innerHTML = '<p class="empty-state">Could not load saved recipes.</p>';
    }
}

async function unsaveRecipe(savedId) {
    if (!confirm('Remove this recipe from your saved list?')) return;

    try {
        await fetch(`/api/saved/${savedId}`, { method: 'DELETE' });
        loadSaved();
    } catch (err) {
        alert('Could not remove recipe.');
    }
}


// Templates tab
async function loadTemplates() {
    const list = document.getElementById('templatesList');
    list.innerHTML = '<p class="empty-state">Loading templates...</p>';

    try {
        const res = await fetch('/api/templates');
        const templates = await res.json();

        list.innerHTML = '';
        templates.forEach(t => {
            const card = document.createElement('div');
            card.className = 'template-card';

            // Items is now an array of objects
            // Each item has name, quantity, unit, days_until_expire
            const itemList = t.items.map(i => i.name).join(', ');

            card.innerHTML = `
                <h4>${escapeHTML(t.name)}</h4>
                <p class="description">${escapeHTML(t.description || '')}</p>
                <div class="template-items"><strong>Includes:</strong> ${escapeHTML(itemList)}</div>
                <button class="btn-primary" style="width:auto; padding: 8px 16px;" onclick="applyTemplate('${t.template_id}', '${escapeForJS(t.name)}')">Add to Pantry</button>
            `;
            list.appendChild(card);
        });
    } catch (err) {
        list.innerHTML = '<p class="empty-state">Could not load templates.</p>';
    }
}

async function applyTemplate(templateId, templateName) {
    if (!confirm(`Add all items from "${templateName}" to your pantry?`)) return;

    try {
        const res = await fetch('/api/templates/apply', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ user_id: userId, template_id: templateId })
        });
        const data = await res.json();
        if (res.ok) {
            alert(`Added ${data.added_count} items to your pantry!`);
        }
    } catch (err) {
        alert('Could not apply template.');
    }
}


// Shopping list tab
async function loadShoppingList() {
    const list = document.getElementById('shoppingList');
    const controls = document.getElementById('shoppingControls');
    list.innerHTML = '<p class="empty-state">Loading shopping list...</p>';

    try {
        const res = await fetch(`/api/shopping/${userId}`);
        const items = await res.json();

        if (items.length === 0) {
            list.innerHTML = '<p class="empty-state">Your shopping list is empty. Add missing ingredients from the Recipes tab!</p>';
            controls.classList.add('hidden');
            return;
        }

        // Show the buttons since list isn't empty
        controls.classList.remove('hidden');

        // Group items by recipe (or "Other" if no recipe linked)
        const grouped = {};
        items.forEach(item => {
            const key = item.recipe_name || 'Other';
            if (!grouped[key]) grouped[key] = [];
            grouped[key].push(item);
        });

        // Build HTML
        list.innerHTML = '';
        Object.keys(grouped).forEach(recipeName => {
            // Section header for the recipe
            const header = document.createElement('div');
            header.className = 'shopping-group-header';
            header.textContent = `For: ${recipeName}`;
            list.appendChild(header);

            // Items in this group
            grouped[recipeName].forEach(item => {
                const div = document.createElement('div');
                div.className = 'shopping-item';
                div.innerHTML = `
                    <span class="shopping-item-name">${escapeHTML(item.name)}</span>
                    <button class="btn-danger" onclick="removeShoppingItem('${item.item_id}')">Remove</button>
                `;
                list.appendChild(div);
            });
        });
    } catch (err) {
        list.innerHTML = '<p class="empty-state">Could not load shopping list.</p>';
        console.log('Load shopping error:', err);
    }
}

// Add missing ingredients from a recipe to the shopping list
// Called from the recipe modal's "Add Missing to Shopping List" button
async function addMissingToShoppingList(missingJson, recipeName) {
    try {
        // Decode the JSON string back into an array
        const missing = JSON.parse(decodeURIComponent(missingJson));

        const res = await fetch('/api/shopping', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({
                user_id: userId,
                items: missing,
                recipe_name: recipeName
            })
        });

        const data = await res.json();
        if (res.ok) {
            if (data.added_count === 0) {
                alert('Those items are already on your shopping list.');
            } else {
                alert(`Added ${data.added_count} item(s) to your shopping list!`);
            }
        }
    } catch (err) {
        alert('Could not add to shopping list.');
        console.log('Add missing error:', err);
    }
}

// Remove a single item from the shopping list
async function removeShoppingItem(itemId) {
    if (!confirm('Remove this item from your shopping list?')) return;

    try {
        const res = await fetch(`/api/shopping/${itemId}`, { method: 'DELETE' });
        if (res.ok) loadShoppingList();
    } catch (err) {
        alert('Could not remove item.');
    }
}

// Mark all shopping list items as bought - moves them all to the pantry
document.getElementById('markAllBoughtBtn').addEventListener('click', async () => {
    if (!confirm('Mark all items as bought? They will be added to your pantry with default quantities.')) return;

    try {
        const res = await fetch('/api/shopping/buy-all', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ user_id: userId })
        });
        const data = await res.json();
        if (res.ok) {
            alert(`Added ${data.added_count} item(s) to your pantry! Check the My Pantry tab to set quantities and expiration dates.`);
            loadShoppingList();
        }
    } catch (err) {
        alert('Could not mark items as bought.');
    }
});

// Clear the entire shopping list without buying
document.getElementById('clearShoppingBtn').addEventListener('click', async () => {
    if (!confirm('Clear your entire shopping list? This cannot be undone.')) return;

    try {
        const res = await fetch(`/api/shopping/clear/${userId}`, { method: 'DELETE' });
        if (res.ok) loadShoppingList();
    } catch (err) {
        alert('Could not clear list.');
    }
});


// Show pantry tab by default when page opens
loadPantry();