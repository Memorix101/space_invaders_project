extends AnimationPlayer

# class member variables go here, for example:
# var a = 2
# var b = "textvar"

func _ready():
	# Called every time the node is added to the scene.
	# Initialization here
	set_process(true)

func _process(delta):
	#print(self.get_current_animation_pos())
	if(self.get_current_animation_pos() >= 1.0):
		#print("lololol")
		get_parent().queue_free()