extends AnimationPlayer

func _ready():
	# Called every time the node is added to the scene.
	# Initialization here
	set_process(true)

func _process(delta):
	#print(self.get_current_animation_pos())
	if(self.get_current_animation_position() >= 1.0):
		#print("lololol")
		get_parent().queue_free()