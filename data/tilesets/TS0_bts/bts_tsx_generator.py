import os


IMAGE_HEIGHT = 256
IMAGE_WIDTH = 256

# Terrain collision regions
TERRAIN_START = (0, 0)
TERRAIN_LAST = (240, 112)

# Container items regions
CONTAINER_START = (0, 128)
CONTAINER_BIG_START = (0, 144)
CONTAINER_LAST = (240, 192)

# Spawners
SPAWNER_SINGLE_START = (0, 208)

# Teleport
TELEPORT_TALL_START = (0, 224)
TELEPORT_WIDE_START = (80, 224)
TELEPORT_WIDE_LAST = (80, 240)

tile_count: int = 0
xml_strings: list[str] = []


def add_tile(x, y, tile_width, tile_height):
    global tile_count, xml_strings
    xml_strings.append(f' <tile id="{tile_count}" x="{x}" y="{y}" width="{tile_width}" height="{tile_height}" type="Tile">\n')
    xml_strings.append(f'  <image source="bts.png" width="{IMAGE_WIDTH}" height="{IMAGE_WIDTH}"/>\n')
    xml_strings.append(f' </tile>\n')
    tile_count += 1
    return

def add_tiles_by_region(tile_width, tile_height, start, end):
    global tile_count, xml_strings
    x: int = start[0]
    y: int = start[1]
    for y in range(start[1], end[1] + tile_height, tile_height):
        for x in range(start[0], end[0] + tile_width, tile_width):
            add_tile(x, y, tile_width, tile_height)
    return

def add_tiles_by_count(tile_width, tile_height, start, count = 1):
    global tile_count
    region_tile_count: int = 0
    x: int = start[0]
    y: int = start[1]
    while (region_tile_count < count):
        while(x < IMAGE_WIDTH and region_tile_count < count):
            add_tile(x, y, tile_width, tile_height)
            region_tile_count += 1
            x += tile_width
        x = 0
        y += tile_height
    return

def main():
    xml_strings.append('<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n')
    xml_strings.append('<tileset>\n')

    # Add tiles by region or count
    add_tiles_by_region(16, 16, TERRAIN_START, TERRAIN_LAST)
    add_tiles_by_count(16, 16, CONTAINER_START, 16)
    add_tiles_by_count(32, 32, CONTAINER_BIG_START, 16)
    add_tiles_by_count(16, 16, SPAWNER_SINGLE_START, 16)
    add_tiles_by_count(16, 32, TELEPORT_TALL_START, 5)
    add_tiles_by_region(32, 16, TELEPORT_WIDE_START, TELEPORT_WIDE_LAST)

    xml_strings[1] = f'<tileset version="1.10" tiledversion="1.11.0" name="TS0_bts" tilewidth="16" tileheight="16" tilecount="{tile_count}" columns="16" objectalignment="topleft">\n'
    xml_strings.append('</tileset>\n')
    target_dir = os.path.dirname(os.path.abspath(__file__))
    with open(f"{target_dir}/TS0_bts.tsx", 'w') as tsx_file:
        tsx_file.writelines(xml_strings)
    return


if __name__ == "__main__":
    main()
