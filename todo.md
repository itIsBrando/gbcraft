# Todo List


### Features
- [ ] Add terrain generation
- [ ] Add vblank interrupt to copy `obj_t` to OAM
- [ ] Allow `bg_write_tile()` to accept coordinates greater than 32
- [ ] Add stone
- [ ] Add supporting for `9pt indexing`
- [ ] Add `hotbar.h`
- [ ] Add player's health to the hotbar
- [ ] Add the player's active item to the hotbar
- [ ] Add `ent_remove(level_t *, ent_t *)`
- [x] prevent duplicates in inventory
- [x] Add `zombie_t`
- [x] Add `inventory_t`
- [x] Add `item_t`
- [x] Support large GBA mapping mode
- [x] change bg map to *affine* mapping
- [x] Add bounding rectangles for entities


### Polish
- [ ] Hide entities that leave visible screen
- [ ] Move & cleanup player's cursor sprite handling
- [ ] Prevent user from placing block on the tile that it occupies
- [ ] Animate player


### Bug
- [ ] Confirm functional collision with other entities
- [ ] Out-of-bounds placing tiles is weird
- [ ] Fix weird cursor positioning
- [ ] Cursor position does not line up with tile placement
- [ ] `bg.h` does not support regular backgrounds with sizes larger than 32x32