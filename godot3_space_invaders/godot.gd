extends Sprite

var time = 0
var reverse = false
var speed = 0.5

func _ready():
	# Called every time the node is added to the scene.
	# Initialization here
	set_process(true)

func _process(delta):
	#print("timeh: ", time)
	if(!reverse):
		time += speed * delta
	if(time >= 1):
		reverse = true	
	if(reverse):
		time -= speed * delta	
	self.modulate.a = time	
	if(reverse && time <= 0):
		get_tree().change_scene("fmg.tscn")