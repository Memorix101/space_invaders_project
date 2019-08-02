extends Area2D

# class member variables go here, for example:
# var a = 2
# var b = "textvar"

var SPEED = -350
var TIME = 0
var HIT = false

func _ready():
	# Called every time the node is added to the scene.
	# Initialization here
	set_physics_process(true)
	add_to_group("entities")
	#get_node("Area2D").connect("area_enter",self,"_on_Area2D_area_enter")	

func _process(delta):	
	TIME += 1 * delta	
	translate(Vector2(0, SPEED * delta))
  
func _set_physics_process(delta):
		
	if(self.get_position().y <= 0):
		self.queue_free() #destroys instance

	#if is_colliding():	# colliding with Static, Kinematic, Rigid after using MOVE
	#	print ("Collision with ", get_collider() )
	#	other.queue_free()
	#	self.queue_free() #destroys instance

func _on_Area2D_body_enter(other):
	if HIT == false:
		#print(other.get_name())
		HIT = true
		other.queue_free()
		var enemy_pos = other.get_position()
		var explo = preload("res://explo.tscn").instance() #bullet prefab
		explo.set_position(Vector2(enemy_pos.x + 75, enemy_pos.y + 75))
		explo.get_node("AnimationPlayer").play("anim");
		get_node("../.").add_child(explo)
		get_tree().get_root().get_node("global").score += 100
		get_tree().get_root().get_node("global").killed_enemies += 1
		self.queue_free() #destroys instance