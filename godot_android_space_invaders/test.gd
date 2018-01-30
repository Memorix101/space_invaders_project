extends Node2D

# class member variables go here, for example:
# var a = 2
# var b = "textvar"

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
		
		var enemy = preload("res://enemy.tscn").instance() #bullet prefab
		enemy.set_pos(Vector2(64 * rowCount, 100 + (50 * row)))
		self.add_child(enemy)
	
	get_tree().get_root().get_node("global").done = true

func _process(delta):
	#moo