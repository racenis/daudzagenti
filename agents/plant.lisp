; PLANT
; This agent simulate a plant.

(name plant)

(mass 250.0)
(position -1.2 1.0 3.2)
(rotation 1.2)
(collision sphere 0.2)

(category plant green leafy)

(flags dynamics-disable)

(chemical (
	(adp 25)
))

(init (do 
	(print "initializing plant, id:" _self)
	(set-model plant)
))

(uninit (print "uninitializing plant, id:" _self))

(tick (do
	;(print "plant glucose: " (measure glucose))
	
	; light absorption
	(if (> 10 (measure light)) (ingest light 1))
	
	; water absorption
	(ingest water 16)
	
	; co2 absorption
	(ingest carbon-dioxide 2)
	
	; photosynthesis 1 and 2
	(react water adp oxygen)
	(react carbon-dioxide atp glucose)
	
	; check if can make fruit!
	(if (< 15 (measure glucose)) (do
		(set fruit-x (+ (get _self position x) (- 0.5 (random 1))))
		(set fruit-y (+ (get _self position y) (- 0.5 (random 1))))
		(set fruit-z (+ (get _self position z) (- 0.5 (random 1))))
	
		(set new-fruit (instantiate fruit-growing at fruit-x fruit-y fruit-z))
	
		(if (not (== new-fruit 0)) (do
			(print "new fruit:" new-fruit)
		))
	))
	
	(eject oxygen 1)
))

(stimulus (ok))

(action (print "plant not interactable"))
