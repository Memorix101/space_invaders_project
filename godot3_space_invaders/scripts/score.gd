extends Label

func _ready():
	# Called every time the node is added to the scene. 
	# Initialization here
	set_process(true)

func _process(delta):	
	self.set_text("%04d" % get_tree().get_root().get_node("global").score)