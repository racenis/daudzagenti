; CHUNGVALIS
; This agent simulates a little chungvalis.

(name chungvalis)

(mass 50.0)
(position 1.0 0.5 0.0)
(rotation 1.57)
(collision sphere 0.2)
(speed 0.05 0.05)

(category chungvalis green smooth)

(flags all-sensor)

(chemical ((atp 1000)))

(init (do 
	(print "initializing chungvalis, id:" _self)
	(set-model chungvalis)

	(perform forward _self)
))

(uninit (print "uninitializing chungvalis, id:" _self))

(tick (do
	;(print "pyruvate:" (measure pyruvate) "oxygen:" (measure oxygen))
	
	(if (> 100 (measure oxygen))
		(ingest oxygen 1))
	
	(react glucose adp pyruvate)

	(react pyruvate oxygen pyruvate-oxide)
	(react pyruvate-oxide adp carbon-dioxide)

	(eject carbon-dioxide 1)
))

(stimulus (do


	if (not (== _self _subject)) (
	

		if (and 
			(is _stim visual)
			(is _subtype appear)
			(is fruit (get _subject category category))
			(is red (get _subject category visual))) (do
		
			(perform push _subject)
			
		)
		
	) (is ok ok) ; just because of bug
))

(action (print "chungvalis not interactable"))
