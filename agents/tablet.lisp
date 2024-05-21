; TABLET
; This agent simulates a sugar tablet that is slowly dissolving.

(name tablet-agent)
(description "A tablet that dissolves glucose.")

(mass 50.0)
(position 8.0 1.0 6.0)
(collision sphere 0.5)

(category tablet colorless smooth)

(chemical ((glucose 100000)))

(init (do (print "initializing tablet, id:" _self) (eject glucose 4) (set-model tablet)))

(uninit (print "uninitializing tablet, id:" _self))

(tick (eject glucose 4))

(stimulus (ok))

(action (print "tablet not interactable"))
