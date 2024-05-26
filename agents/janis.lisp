; JANIS
; This agent simulates a little janis.

(name janis)

(mass 50.0)
(position 0.0 0.5 1.0)
(collision sphere 0.5)
(speed 0.05 0.05)

(category janis green smooth)

(flags all-energy all-sensor)

(init (do 
	(print "initializing janis, id:" _self)
	(set-model janis)
	(set state swimming)
	(perform forward _self)
))

(uninit (print "uninitializing janis, id:" _self))

(tick (do

	(if (is state swimming)
		(if (> 0.05 (random 1)) (do 
			(set state turning)
			
			(if (> 0.5 (random 1))
				(perform left _self)
				(perform right _self)))))

	(if (is state turning)
		(if (> 0.04 (random 1)) (do 
			(set state swimming)
			
			(perform forward _self))))

))

(stimulus (ok))

(action (print "janis not interactable"))
