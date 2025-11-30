ack = (lam m: N. rec (S) (lam mp: N. lam f: (N -> N). lam n: N. rec n (f (S 0)) (lam np: N. lam r: N. f r)) (m))

main = ack 0 5
