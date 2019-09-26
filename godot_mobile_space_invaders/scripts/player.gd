extends Node

# class member variables go here, for example:
# var a = 2
# var b = "textvar"

var SPEED = 350
var BUTTON_ACTIVE = false

func _ready():
	# Called every time the node is added to the scene.
	# Initialization here
	# pass
	set_process(true)
	set_fixed_process(true)

func _fixed_process(delta):
    # or actions: Input.is_action_just_pressed("ui_up")
		var player_pos = self.get_pos()

		#limit movement axis
		player_pos.x = clamp(player_pos.x, 30, 995)

		#shooting lasers
		if(get_parent().get_node("Node/Button_Shoot").is_pressed() && BUTTON_ACTIVE == false):
			BUTTON_ACTIVE = true
			var bullet = preload("res://prefabs/bullet.tscn").instance() #bullet prefab
			bullet.set_pos(Vector2(player_pos.x, player_pos.y - 50))
			get_node("../..").add_child(bullet)
		elif(get_parent().get_node("Node/Button_Shoot").is_pressed() == false && BUTTON_ACTIVE):
			BUTTON_ACTIVE = false

		#moving player
		if(Input.is_key_pressed(KEY_LEFT) || get_parent().get_node("Node/Button_Left").is_pressed()):
			player_pos.x -= delta*SPEED
		elif(Input.is_key_pressed(KEY_RIGHT) || get_parent().get_node("Node/Button_Right").is_pressed()):
			player_pos.x += delta*SPEED

		#update pos
		self.set_pos(player_pos)
