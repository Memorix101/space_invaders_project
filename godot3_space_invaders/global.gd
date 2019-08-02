extends Node

var done = false #test.gd enemy spawn
var score = 0
var killed_enemies = 0
var gameover = false

func _ready():
	# Called every time the node is added to the scene.
	# Initialization here
	pass

func restart():
	print("restarting level ... ")
	for i in get_tree().get_nodes_in_group("entities"):
    		i.queue_free()
	get_tree().change_scene("test.tscn")