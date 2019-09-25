extends Area2D

var SPEED = 350
var TIME = 0

func _ready():
	# Called every time the node is added to the scene.
	# Initialization here
	set_process(true)
	set_physics_process(true)
	add_to_group("entities")
	#get_node("Area2D").connect("area_enter",self,"_on_Area2D_area_enter")

func _process(delta):	
	TIME += 1 * delta	
	translate(Vector2(0, SPEED * delta))
  
func _set_physics_process(delta):	
	if(self.get_position().y >= 600):
		self.queue_free() #destroys instance
	#if is_colliding():	# colliding with Static, Kinematic, Rigid after using MOVE
	#	print ("Collision with ", get_collider() )
	#	other.queue_free()
	#	self.queue_free() #destroys instance

func _on_Area2D_body_enter( other ):
	#print(other.get_name())
	if(other.get_name() == "Player"):
		var explo = preload("res://prefabs/explo.tscn").instance() #prefab
		explo.set_position(Vector2(other.get_position().x + 100, other.get_position().y + 24))
		explo.get_node("AnimationPlayer").play("anim")
		get_tree().get_root().add_child(explo)		
		other.queue_free()	
		self.queue_free() #destroys instance
		#call_deferred("queue_free()")		
		get_tree().get_root().get_node("global").gameover = true