; FORBIDDEN GHERKIN
; This agent simulates a little gherkin that searches for sugar.

(name forbidden-gherkin-agent)

(mass 50.0)
(position 1.0 1.0 1.0)
(collision sphere 0.5)
(speed 0.05 0.05)

(category gherkin green bumpy)

(flags all-energy all-sensor)

(init (do 
	(print "initializing gherkin, id:" _self)
	(set-model forbidden-gherkin)
	(set state swimming)
	(perform forward _self)
	
	(set prev (list 0))
))

(uninit (print "uninitializing gherkin, id:" _self))

(tick (do

	; add current glucose level to the memory
	(set prev (list push prev (probe glucose)))
	
	; if memory queue longer than 30, shorten it
	(if (< 30 (list size prev)) (list drop prev))
	
	; calculate delta glucose
	(set delta (- (probe glucose) (list last prev)))
	
	(if (not (== delta 0)) (print "glucose:" (probe glucose) "delta:" delta))	
	
	; avoid division with zero
	(if (== delta 0) (set delta 0.05))
	
	(if (is state swimming)
		(if (> (/ 0.05 delta) (random 1)) (do 
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

(stimulus (ok))

(action (print "gherkin not interactable"))
