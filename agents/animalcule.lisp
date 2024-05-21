; ANIMALCULE
; This agent simulates a little animalcule that avoids obstacles.

(name animalcule-agent)

(mass 50.0)
(rotation -1.57)
(position 1.0 1.0 5.0)
(collision sphere 0.5)

(speed 0.05 0.05)

(category animalcule green smooth)

(chemical ((atp 1000)))

(init (do 
	(print "initializing animalcule, id:" _self)
	(set-model animalcule)
	(set state swimming)
	(perform forward _self)
))

(uninit (print "uninitializing animalcule, id:" _self))

(tick (do

	(if (is state retreating)
		(if (> 0.05 (random 1)) (do 
			(set state turning)
			
			(if (> 0.5 (random 1))
				(perform left _self)
				(perform right _self))
			
			(print "ok turning now"))))

	(if (is state turning)
		(if (> 0.04 (random 1)) (do 
			(set state swimming)
			(perform forward _self)
			(print "ok swimming now"))))

))

(stimulus (do
	
	(if (and (is _stim touch) (is _subtype agent)) (do
		(set state retreating)
		(perform backward _self)
		(print "ok retreating" agent _self)))

))

(action (print "animalcule not interactable"))
