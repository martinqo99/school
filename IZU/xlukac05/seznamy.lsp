;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Autor: Michal Lukac, xlukac05@stud.fit.vutbr.cz        ;;
;; Predmet: Praca so zoznamami do predmetu IZU na VUT FIT ;;
;; Datum: 21.4.2012                                       ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; spojeni seznamu
(defun spoj (L1 L2)
  (cond ((NULL L1) L2)
        (T (cons (car L1) (spoj (cdr L1) L2)))
  )
)

; vytvorte
; obraceni obecneho seznamu obratOb(seznam)
(defun obratOb (L1)
  (cond ((equal NIL (car L1)) () )
        ((atom (car L1)) (spoj (obratOb (cdr L1)) (list (car L1))) )
        (t (spoj (obratOb (cdr L1)) (list (obratOb (car L1)))) )
  )
)

; test zda je prvek obsazen v obecnem seznamu jePrvekOb(seznam, prvek)
(defun jePrvekOb (L1 IT)
  (cond ((equal NIL (car L1)) () )
        ((atom (car L1)) (or (jePrvekOb (cdr L1) IT) (= IT (car L1))) )
        (t (or (jePrvekOb (car L1) IT) (jePrvekOb (cdr L1) IT)) )
  )
)

; prumer prvku obecneho seznamu prumerPrvkuOb(seznam)
(defun prumerPrvkuOb(L1)
  (cond ((equal NIL (car L1)) 0 )
        (t 0)
  )
)

; soucet prvku obecneho seznamu soucetPrvkuOb(seznam)
(defun soucetPrvkuOb(L1)
  (cond ((equal NIL (car L1)) 0 ) 
        ((atom (car L1)) (+ (soucetPrvkuOb (cdr L1)) (car L1)) ) 
        (t (+ (soucetPrvkuOb (car L1)) (soucetPrvkuOb (cdr L1))) )
  )
)

; overeni monotonnosti linearniho seznamu monotonnostLin(seznam)
(defun monotonnostLin(L1)
  (cond ((equal NIL (car L1)) 0 )
        (t 0)
  )
)
