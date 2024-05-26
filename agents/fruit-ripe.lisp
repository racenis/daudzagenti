; FRUIT-RIPE
; This agent simulates a fruit that is ripe.

(name fruit-ripe)
(description "Fruit that is not ripe yet.")

(mass 1.0)
(position 5.0 0.01 5.0)
(collision sphere 0.1)

(category fruit red round)

;(flags dynamics-disable)

(chemical ((glucose 25)))

(init (do 
	(print "initializing fruit-ripe, id:" _self)
	(set-model fruit)
	(set-color 0.9999 0.0 0.0)))

(uninit (print "uninitializing fruit-ripe, id:" _self))


(action (do

	(if (is _action push) (do
		
		(inject _subject glucose 10)

		(kill _self)))
	
))