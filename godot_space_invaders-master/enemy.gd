extends KinematicBody2D

# class member variables go here, for example:
var SPEED = 250
var TIME = 0;
var START_POS = 0;

var move_left = false
var move_right = true

var fire_rate = rand_range(3, 20) #seconds
var fire_timer = 0

func _ready():
	# Called every time the node is added to the scene.
	# Initialization here
	set_process(true)
	START_POS = self.get_pos().x

func _process(delta):
	
	var enemy_pos = self.get_pos()
	
	#limit movement axis
	#enemy_pos.x = clamp(enemy_pos.x, 26, 1000)
	
	#print("epos: ", self.get_pos().x)
	#print("le: ", move_left, " ri: ", move_right, "sp: ", SPEED)
	
	fire_timer += 1 * delta
	
	if(fire_timer >= fire_rate):		
		#spawn bulltet
		var enemy_bullet = preload("res://enemy_bullet.tscn").instance() #bullet prefab
		enemy_bullet.set_pos(Vector2(enemy_pos.x, enemy_pos.y + 50))
		get_node("../.").add_child(enemy_bullet)
		#reset
		fire_timer = 0
	
	if( self.get_pos().x <= START_POS && move_left):
		# move right
		move_right = true
		move_left = false
		SPEED = SPEED * -1
	elif(self.get_pos().x >= START_POS + 500 && move_right):
		#move left
		move_right = false
		move_left = true
		SPEED = SPEED * -1

	if(get_tree().get_root().get_node("global").done):
		translate(Vector2(SPEED * delta, 0))