extends Node2D

var row = 0
var rowCount = 0

func _ready():
	# Called every time the node is added to the scene.
	# Initialization here
	_start()
	set_process(true)

func _start():
	for i in range(40):
		#print("Hi ", i)
		rowCount += 1		
		if(rowCount == 9):
			rowCount = 1
			row += 1		
		var enemy = preload("res://enemy.tscn").instance() #prefab
		enemy.set_position(Vector2(64 * rowCount, 100 + (50 * row)))
		self.add_child(enemy)	
	get_tree().get_root().get_node("global").done = true

func _process(delta):	
	#print(get_tree().get_root().get_node("global").killed_enemies)		
	if get_tree().get_root().get_node("global").killed_enemies >= 40:
		get_tree().get_root().get_node("global").gameover = true
	
	if get_tree().get_root().get_node("global").killed_enemies >= 40 and get_tree().get_root().get_node("global").gameover == true:
		get_node("win_ui").show()
		
	if get_tree().get_root().get_node("global").killed_enemies < 40 and get_tree().get_root().get_node("global").gameover == true:
		get_node("gameover_ui").show()
		
	if Input.is_action_pressed("quit_game"):
		get_tree().quit() # default behavior
	
	if get_tree().get_root().get_node("global").gameover == true and Input.is_action_pressed("ui_accept"):
		get_node("gameover_ui").hide()
		get_node("win_ui").hide()
		get_tree().get_root().get_node("global").score = 0
		get_tree().get_root().get_node("global").killed_enemies = 0
		get_tree().get_root().get_node("global").gameover = false
		#remove all active nodes
		get_tree().get_root().get_node("global").restart()
		#for i in get_children():
    	#	i.queue_free()
		#get_tree().change_scene("test.tscn")