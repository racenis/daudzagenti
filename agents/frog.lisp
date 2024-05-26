; FROG
; This agent simulates a frog.

(name frog)
(description "A small, immovable frog.")

(mass 15.0)
(position -1.5 0.2 -1.5)
(rotation 4.0)
(collision sphere 0.2)

(category frog green smooth)

(flags dynamics-disable)

(init (do (set-model frog)))
