#lang racket
(require "../../books-programming.raw")
(require "../../books-security.raw")
(require "../../books-ot.raw")

(define (_add-num acc xs n)
  (if (null? xs) acc (_add-num (cons (cons n (car xs)) acc) (cdr xs) (+ 1 n))))

(define (add-num xs)
  (reverse (_add-num '() xs 1)))

(define (colour-pages in)
  (let ([pages (regexp-match #px"(\\d+)\\s*/\\s*(\\d+)" in)])
    (match pages
      [(list _ p t) (if (equal? p t) 
                      (format "<span style=\"color:green;\">~a/~a</span>" p t)
                      (format "<span style=\"color:red;\">~a/~a</span>" p t))]
      [_ ""])))

(define (print-book-table books)
  (letrec ([books-sorted (sort books (λ (a b) 
                                    (string<? 
                                      (~a (car a)) 
                                      (~a (car b)))))]
           [books-sorted-numbered (add-num books-sorted)])
    (printf "| # | Title | Author | Pages | Skills | ~n")
    (for* ([i books-sorted-numbered])
        (printf "| ~a | ~a | ~a | ~a | ~a |~n" 
               (car i) (cadr i) (caddr i) (colour-pages (cadddr i)) (fifth i)))))


;(print-book-table books-programming)
;(print-book-table books-security)
;(print-book-table books-other)

(define lists-hash (hash
                     "books-programming" books-programming
                     "books-security" books-security
                     "books-other" books-other))

(for* ([l (current-command-line-arguments)])
      (print-book-table (hash-ref lists-hash l)))
;(define t (hash '("hello" "world")))
;(hash-ref t "hello")
;(match (current-command-line-arguments)
;  [(vector "books-programming") (displayln "a")]
;  [(vector "books-security") (displayln "b")]
;  [(vector "books-other") (displayln "c")]
;  
;  )

