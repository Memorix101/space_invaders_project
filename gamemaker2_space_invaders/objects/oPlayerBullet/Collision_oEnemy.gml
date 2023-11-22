/// @description Insert description here
// You can write your code in this editor


instance_create_layer(other.x - 45, other.y - 45, "bullets", oExplode)
instance_destroy(self);
instance_destroy(other);
score = score + 100;