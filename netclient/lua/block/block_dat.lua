
--print("map_load_tiles: loading tiles") ;


--[[
t top
b bottom
n north
s south
w west
e east
]]


if (IS_CLIENT == 0) then 
	function texture_alias()
		return nil
	end

	function register_spritesheet()
		return nil
	end
	function register_texture()
		return nil
	end
end

--- convenience method

function iso_texture(spritesheet, xpos, ypos)
	if(IS_CLIENT == 0) then return {t = nil, b = nil, n = nil, s = nil, w = nil, e = nil} end;
	local id;
	if (type(spritesheet) == "number" and xpos == nil and ypos == nil) then
		id = spritesheet
	else
		if(not xpos or not ypos) then error("iso_texture: xpos or ypos not set!") end
		id = register_texture(spritesheet, xpos, ypos)
	end
	return  { t = id, b = id, n = id, s = id, w = id, e = id };
end

function NewSolidBlock(id, name)
  o = BlockNew(id, name)
  o.properties = SolidBlockProperty;
  return o
end

--register texture sheets
t00 = register_spritesheet("t00")
t01 = register_spritesheet("t01")

--alias texture
error_block = texture_alias(t00,0,0)
black_blow = texture_alias(t00,3,1)
stone_block = texture_alias(t00,0,2)





Block:new1(0, "empty", EmptyBlockProperty, iso_texture(error_block), NoHud);


--- Classic Blocks ---

b = NewSolidBlock(255, "error_block"); -- id, name
b.texture = iso_texture(error_block);
b.hud = hud(0, error_block);

b = NewSolidBlock(1, "lava");
b.texture = iso_texture(t00,1,0);
b.hud = hud(3, b.texture.n);

b = NewSolidBlock(2, "terminal_blue");
b.texture = iso_texture(t00,0,3);
b.hud = hud(4, b.texture.n);

b = NewSolidBlock(3, "terminal_green");
b.texture = iso_texture(t00,0,3);
b.hud = hud(8+0, b.texture.n);

b = NewSolidBlock(4, "solar_panel");
b.texture = iso_texture(t00,0,1);
b.hud = hud(8+1, b.texture.n);

b = NewSolidBlock(5, "battery");
b.texture = iso_texture(t00,2,1);
b.texture.t = register_texture(t00,3,1);
b.texture.b = register_texture(t00,1,1);
b.hud = hud(8+2, b.texture.n);
b.max_damage = 32;

b = NewSolidBlock(6, "sand");
b.texture = iso_texture(t00,0,3);
b.hud = hud(8+3, b.texture.n);

b = NewSolidBlock(7, "sand_brick");
b.texture = iso_texture(t00,2,2);
b.hud = hud(16+0, b.texture.n);

b = NewSolidBlock(8, "red_brick");
b.texture = iso_texture(t00,1,2);
b.hud = hud(16+1, b.texture.n);

b = NewSolidBlock(9, "grey_brick");
b.texture = iso_texture(t00,3,2);
b.hud = hud(16+2, b.texture.n);

b = NewSolidBlock(10, "rough_stone");
b.texture = iso_texture(t00,0,2);
b.hud = hud(16+3, b.texture.n);

b = NewSolidBlock(11, "rough_stone_blue_ore");
b.texture = iso_texture(t00,1,3);
b.hud = hud(16+3, b.texture.n);

b = NewSolidBlock(12, "wood_panel");
b.texture = iso_texture(t00,2,3);
b.hud = hud(16+4, b.texture.n);

-- cell blocks --

b = NewSolidBlock(16, "holy_stone");
b.texture = iso_texture(t01,5,4);
b.hud = hud(12, t01,5,4);
print("map_load_tiles: done");

--[[
for i, block in pairs(block_id_table) do
  o = {}
end
]]