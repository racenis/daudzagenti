; SEA TABLET
; This agent dumps some chemicals into the sea.

(name sea-tablet)

(mass 50.0)
(position 0.0 2.0 0.0)
(collision sphere 0.5)

(category tablet colorless smooth)

(chemical (
	(water 100000)
	(light 100000)
	(carbon-dioxide 100000)
))

(tick (do 
	(print "ejecting chemicals!")
	(eject water 100000)
	(eject light 100000)
	(eject carbon-dioxide 100000)

	;(print "inserted:" (instantiate obstacle-agent at 0.0 0.0 0.0))
	
	(kill _self)
))
