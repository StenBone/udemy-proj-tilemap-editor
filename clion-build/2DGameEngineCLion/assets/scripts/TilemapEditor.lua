local window_hieght = 600

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
        }
    }
}
