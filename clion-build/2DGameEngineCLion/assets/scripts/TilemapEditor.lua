local window_hieght = 736
local HINTS_START_X = 460
local HINTS_FONT_SPACING = 15
local HINTS_START_Y = window_hieght - (HINTS_FONT_SPACING * 5) --4 hints with a buffer at the bottom


TilemapEditor = {
    ----------------------------------------------------
    -- Table to define the list of assets
    ----------------------------------------------------
    assets = {
        [0] = { type="texture", id = "terrain-texture-day", file = "../assets/tilemaps/jungle.png" },
        [1] = { type="texture", id = "terrain-texture-night", file = "../assets/tilemaps/jungle-night.png" },
        [2] = { type="texture", id = "frame-texture", file = "../assets/images/hr.png" },
        [3] = { type="texture", id = "icon-texture", file = "../assets/images/icon.png" },
        [4] = { type="texture", id = "pointer-texture", file = "../assets/images/pointer.png" },
        [5] = { type="font", id = "hermit-font", file = "../assets/fonts/Hermit-Bold.ttf", fontSize = 10 }
    },

    ----------------------------------------------------
    -- table to define entities and their components
    ----------------------------------------------------
    entities = {
        [0] = {
            name = "frame",
            layer = 6,
            components = {
                transform = {
                    position = {
                        x = 0,
                        y = window_hieght - 128
                    },
                    velocity = {
                        x = 0,
                        y = 0
                    },
                    width = 800,
                    height = 128,
                    scale = 1,
                    rotation = 0
                },
                sprite = {
                    textureAssetId = "frame-texture",
                    animated = false,
                    frameCount = 0,
                    animationSpeed = 0,
                    hasDirections = false,
                    fixed = true --important var
                }
            }
        },
        [1] = {
            name = "tileSelector",
            layer = 6,
            components = {
                transform = {
                    position = {
                        x = 0,
                        y = window_hieght - 96
                    },
                    velocity = {
                        x = 0,
                        y = 0
                    },
                    width = 320,
                    height = 96,
                    scale = 1,
                    rotation = 0
                },
                sprite = {
                    textureAssetId = "terrain-texture-day",
                    animated = false,
                    frameCount = 0,
                    animationSpeed = 0,
                    hasDirections = false,
                    fixed = true --important var
                }
            }
        },
        [2] = {
            name = "tilePreview",
            layer = 6,
            components = {
                transform = {
                    position = {
                        x = 361 + 4, --4 is an offset to center the image
                        y = window_hieght - 128 + 56 + 3 --3 is an offset
                    },
                    velocity = {
                        x = 0,
                        y = 0
                    },
                    width = 32,
                    height = 32,
                    scale = 1,
                    rotation = 0
                },
                sprite = {
                    textureAssetId = "terrain-texture-day",
                    animated = false,
                    frameCount = 0,
                    animationSpeed = 0,
                    hasDirections = false,
                    fixed = true --important var
                }
            }
        },
        [3] = {
            name = "hints-heading",
            layer = 6,
            components = {
                textLabel = {
                    x = HINTS_START_X,
                    y = HINTS_START_Y - 25,
                    text = "TILEMAP EDITOR",
                    fontFamily = "hermit-font"
                }
            }
        },
        [4] = {
            name = "hint-1",
            layer = 6,
            components = {
                textLabel = {
                    x = HINTS_START_X,
                    y = HINTS_START_Y + (HINTS_FONT_SPACING * 0),
                    text = "1. click one of the tiles to select",
                    fontFamily = "hermit-font"
                }
            }
        },
        [5] = {
            name = "hint-2",
            layer = 6,
            components = {
                textLabel = {
                    x = HINTS_START_X,
                    y = HINTS_START_Y + (HINTS_FONT_SPACING * 1),
                    text = "2. use the mouse to draw on the map",
                    fontFamily = "hermit-font"
                }
            }
        },
        [6] = {
            name = "hint-3",
            layer = 6,
            components = {
                textLabel = {
                    x = HINTS_START_X,
                    y = HINTS_START_Y + (HINTS_FONT_SPACING * 2),
                    text = "3. press \"w\" to write the map file",
                    fontFamily = "hermit-font"
                }
            }
        },
        [7] = {
            name = "hint-4",
            layer = 6,
            components = {
                textLabel = {
                    x = HINTS_START_X,
                    y = HINTS_START_Y + (HINTS_FONT_SPACING * 3),
                    text = "4. press \"q\" to quit the editor",
                    fontFamily = "hermit-font"
                }
            }
        }
    }
}
