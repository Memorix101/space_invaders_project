extends Sprite

# Called when the node enters the scene tree for the first time.
func _ready():
	set_process(true)
	add_to_group("entities")

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass