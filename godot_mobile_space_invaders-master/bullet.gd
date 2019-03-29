extends Area2D

# class member variables go here, for example:
# var a = 2
# var b = "textvar"

var SPEED = -250
var TIME = 0;

func _ready():
	# Called every time the node is added to the scene.
	# Initialization here
	set_fixed_process(true)
	#get_node("Area2D").connect("area_enter",self,"_on_Area2D_area_enter")

func _process(delta):
	TIME += 1 * delta
  
func _fixed_process(delta):
	
	translate(Vector2(0, SPEED * delta))
	
	if(self.get_pos().y <= 0):
		self.queue_free() #destroys instance

	#if is_colliding():	# colliding with Static, Kinematic, Rigid after using MOVE
	#	print ("Collision with ", get_collider() )
	#	other.queue_free()
	#	self.queue_free() #destroys instance

func _on_Area2D_body_enter( other ):
	    #print(other.get_name())
	other.queue_free()
	
	var explo = preload("res://explo.tscn").instance() #bullet prefab
	explo.set_pos(other.get_pos())
	explo.get_node("AnimationPlayer").play("anim");
	get_node("../.").add_child(explo)
	
	get_tree().get_root().get_node("global").score += 100
	
	self.queue_free() #destroys instance
