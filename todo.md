# Todo List


### Features
- [ ] Add terrain generation
- [ ] Add slime entity
- [ ] Add real player tracking to zombie
- [ ] Add player side sprite
- [ ] Add player back sprite
- [ ] Add tree sprite
- [ ] Add tree tile
- [ ] Add crafting table
- [ ] Add chest
- [ ] Add *powerglove*
- [ ] Require block breaking to use multiple hits
- [ ] Add stamina to hotbar
- [ ] Subtract from stamina upon using a tool
- [ ] Replenish stamina over time
- [ ] Add pointer to player entity in `level_t`
- [ ] Add vblank interrupt to copy `obj_t` to OAM
- [ ] Allow `bg_write_tile()` to accept coordinates greater than 32
- [ ] Add supporting for `9pt indexing`
- [ ] Add `hotbar.h`
- [ ] Add `ent_remove(level_t *, ent_t *)`
- [x] Add player's health to the hotbar
- [x] Add stone
- [x] Add axe
- [x] Add inventory GUI
- [x] Add the player's active item to the hotbar
- [x] prevent duplicates in inventory
- [x] Add `zombie_t`
- [x] Add `inventory_t`
- [x] Add `item_t`
- [x] Support large GBA mapping mode
- [x] change bg map to *affine* mapping
- [x] Add bounding rectangles for entities


### Polish
- [ ] Move & cleanup player's cursor sprite handling
- [ ] Prevent user from placing block on the tile that it occupies
- [ ] Animate player
- [x] Hide entities that leave visible screen


### Bug
- [ ] Confirm functional collision with other entities
- [ ] Out-of-bounds placing tiles is weird
- [ ] Fix weird cursor positioning
- [ ] Cursor position does not line up with tile placement
- [ ] Game crashes opening inventory menu with no items
- [ ] `bg.h` does not support regular backgrounds with sizes larger than 32x32