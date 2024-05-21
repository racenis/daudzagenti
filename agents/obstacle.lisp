; OBSTACLE
; This agent simulates an obstacle.

(name obstacle-agent)
(description "A small, immovable obstacle.")

(mass 1000.0)
(position 5.0 0.01 5.0)
(collision sphere 1.2)

(category stone gray spiky)

(flags dynamics-disable)


(init (do (print "initializing obstacle, id:" _self) (set-model obstacle)))

(uninit (print "uninitializing obstacle, id:" _self))

(tick (ok))

(stimulus (ok))

(action (print "obstacle not interactable"))
