extends KinematicBody2D

var SPEED = 350
var BUTTON_ACTIVE = false

func _ready():
	# Called every time the node is added to the scene.
	# Initialization here
	# pass
	set_process(true)
	set_physics_process(true)

func _process(delta):
 # or actions: Input.is_action_just_pressed("ui_up")
	var player_pos = self.get_position()
	#limit movement axis
	player_pos.x = clamp(player_pos.x, 30, 995)
	#shooting lasers
	if Input.is_action_pressed("fire") && BUTTON_ACTIVE == false:
		BUTTON_ACTIVE = true
		var bullet = preload("res://bullet.tscn").instance() #bullet prefab
		bullet.set_position(Vector2(player_pos.x + 85, player_pos.y - 5))
		get_tree().get_root().add_child(bullet)
	elif !Input.is_action_pressed("fire") && BUTTON_ACTIVE:
		BUTTON_ACTIVE = false
	#moving player
	if Input.is_action_pressed("ui_left"):
		player_pos.x -= delta*SPEED
	elif Input.is_action_pressed("ui_right"):
		player_pos.x += delta*SPEED
	#update pos
	self.set_position(player_pos)