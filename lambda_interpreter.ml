type lambda_term =
  | Var of string
  | Lambda of string * lambda_term
  | Appl of lambda_term * lambda_term

(* pretty printing *)
let rec pp_aux (_M : lambda_term) (indent : int) : unit =
  for i = 0 to indent do print_string "    " done;
  match _M with
    | Var x -> print_string x
    | Lambda (x, _M') -> print_string "^ "; print_string x; print_string "."; print_newline (); pp_aux _M' (indent + 1)
    | Appl (_U, _V) -> print_string "@"; print_newline (); pp_aux _U (indent + 1); print_newline (); pp_aux _V (indent + 1)

let pp (_M : lambda_term) : unit = pp_aux _M 0

(* Constructors *)

let rec f_itere (n : int) (x : string) (f : string) = match n with
  | 0 -> Var x
  | k -> Appl (Var f, f_itere (k-1) x f)

let lambda_nat (n : int) (x : string) (f : string) = Lambda (f, Lambda(x, f_itere n x f))

(* fv(N) n bv(M) = O *)
let count = ref 0

let fresh (() : unit) : int = incr count; (!count - 1)

let create (() : unit) : ('a * 'b) list = []

let rec find_opt (u : ('a * 'b) list) (key : 'a) : 'b option = match u with
  | [] -> None
  | h::t when (fst h) = key -> Some (snd h)
  | h::t -> find_opt t key

let add (u : ('a * 'b) list) (key : 'a) (value : 'b) : ('a * 'b) list = (key, value)::u

let alpha_rename (_M : lambda_term) : lambda_term =
  let rec renew_aux (mem : (string * int) list) = function
    | Var y -> begin match find_opt mem y with
                | None -> Var y
                | Some x -> Var ("$" ^ (string_of_int x))
               end
    | Lambda (y, _U) -> let tmp = fresh () in Lambda ("$" ^ (string_of_int tmp), renew_aux (add mem y tmp) _U)
    | Appl (_U, _V) -> Appl (renew_aux mem _U, renew_aux mem _V)
  in
  renew_aux (create ()) _M

(* returns M[x := N] *)
let substitution (_M : lambda_term) (x : string) (_N : lambda_term) =
  let rec substitution_aux (_M : lambda_term) (x : string) (_N : lambda_term) : lambda_term = match _M with
    | Var y -> if x = y then _N else Var y
    | Lambda (y, _U) -> if x = y then begin print_string "LFKJDSLKJFLKSDFJSDKLF"; print_newline (); Lambda (y, _U) end else Lambda (y, substitution_aux _U x _N)
    | Appl (_U, _V) -> Appl (substitution_aux _U x _N, substitution_aux _V x _N)
  in 
  substitution_aux (alpha_rename _M) x _N

let rec call_by_name_evaluation (_M : lambda_term) : lambda_term = match _M with
  | Appl (_U, _V) ->
    begin
      let _U' = call_by_name_evaluation _U in
      match _U' with
      | Lambda (x, _T) -> call_by_name_evaluation (substitution _T x _V)
      | _ -> Appl (_U', call_by_name_evaluation _V)
    end
  | _ -> _M

let rec interpreter (_M : lambda_term) : lambda_term =
  match call_by_name_evaluation _M with
    | Lambda (x, _M') -> Lambda (x, interpreter _M')
    | _U -> _U

let rec char_lst_of_string str = function
  | 0 -> []
  | n -> (str.[n-1])::(char_lst_of_string str (n-1))

let string_of_char_lst str =
  let n = List.length str in
  let t = Array.of_list str in
  String.init n (fun x -> t.(x))

let parser () =
  let chan = open_in "test" in
  let str = (input_line (chan)) in
  let n = String.length str in
  let rec parse_id acc = function
    | [] -> (string_of_char_lst acc, [])
    | ' '::t -> (string_of_char_lst acc, ' '::t)
    | h::t -> parse_id (h::acc) t
  in
  let rec parser_aux = function 
      | 'L'::t -> let tmp = parse_id [] (List.tl t) in let body = parser_aux (List.tl (snd tmp)) in (Lambda (fst tmp, fst body), snd body)
      | '^'::t -> let tmp = parse_id [] (List.tl t) in (Var (fst tmp), snd tmp)
      | '@'::t -> let tmp1 = parser_aux (List.tl t) in let tmp2 = parser_aux (List.tl (snd tmp1)) in  (Appl (fst tmp1, fst tmp2), snd tmp2)
      | _ -> failwith "fhjhjbjkl"
  in
  fst (parser_aux (List.rev (char_lst_of_string str n)))

let _ = let tmp = parser () in pp tmp; print_newline (); pp (interpreter tmp)
