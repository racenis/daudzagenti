; FRUIT-GROWING
; This agent simulates a fruit that is growing.

(name fruit-growing)
(description "Fruit that is not ripe yet.")

(mass 1.0)
(position 5.0 0.01 5.0)
(collision sphere 0.1)

(category fruit green round)

(flags dynamics-disable)

(chemical ((glucose 25)))

(init (do 
	(print "initializing fruit-growing, id:" _self)
	(set-model fruit)
	(set-color 0.0 0.9999 0.0)
	(set progress 0)))

(uninit (print "uninitializing fruit-growing, id:" _self))

(tick (do

	(if (< 50 progress) 
		(set-color (- (/ progress 50.0) 1.0) (- 2.0 (/ progress 50.0)) 0.0)
		(set-scale (/ (+ 1 progress) 50.0)))

	

	(if (< 100 progress) (do
		(print "ok done!!!!!!!!!!!!!!!!!!")

		(instantiate fruit-ripe at (get _self position x) (get _self position y) (get _self position z))
		(kill _self)))
		
	(set progress (+ progress 1))
))

(action (print "fruit-growing not interactable"))
