#include "alg.h"

/* Local functions ***********************************************************/

static bool        allowed_HTM(Move m);
static bool        allowed_URF(Move m);
static bool        allowed_eofb(Move m);
static bool        allowed_drud(Move m);
static bool        allowed_drud_noD(Move m);
static bool        allowed_htr(Move m);
static bool        allowed_next_HTM(Move l2, Move l1, Move m);
static int         axis(Move m);

static void        free_alglistnode(AlgListNode *aln);
static void        realloc_alg(Alg *alg, int n);

static int         niss_type(Alg *a);
static void        find_last_moves(Alg *a, bool inv, int *, int *, int *);
static int64_t     last_move_pair(Alg *a, bool inv);
static int         compare_algs_firstmoves(Alg * a, Alg *b, bool inv);
static int         compare_algs(const void * a, const void *b);

/* Movesets ******************************************************************/

Moveset
moveset_HTM = {
	.allowed      = allowed_HTM,
	.allowed_next = allowed_next_HTM,
};

Moveset
moveset_URF = {
	.allowed      = allowed_URF,
	.allowed_next = allowed_next_HTM,
};

Moveset
moveset_eofb = {
	.allowed      = allowed_eofb,
	.allowed_next = allowed_next_HTM,
};

Moveset
moveset_drud = {
	.allowed      = allowed_drud,
	.allowed_next = allowed_next_HTM,
};

Moveset
moveset_drud_noD = {
	.allowed      = allowed_drud_noD,
	.allowed_next = allowed_next_HTM,
};

Moveset
moveset_htr = {
	.allowed      = allowed_htr,
	.allowed_next = allowed_next_HTM,
};

static int nmoveset = 6;
static Moveset * all_ms[] = {
	&moveset_HTM,
	&moveset_URF,
	&moveset_eofb,
	&moveset_drud,
	&moveset_drud_noD,
	&moveset_htr,
};

/* Functions *****************************************************************/

static bool
allowed_HTM(Move m)
{
	return m >= U && m <= B3;
}

static bool
allowed_URF(Move m)
{
	Move b = base_move(m);

	return b == U || b == R || b == F;
}

static bool
allowed_eofb(Move m)
{
	Move b = base_move(m);

	return b == U || b == D || b == R || b == L ||
	       ((b == F || b == B) && m == b+1);
}

static bool
allowed_drud(Move m)
{
	Move b = base_move(m);

	return b == U || b == D ||
	       ((b == R || b == L || b == F || b == B) && m == b + 1);
}

static bool
allowed_drud_noD(Move m)
{
	Move b = base_move(m);

	return b == U ||
	       ((b == R || b == L || b == F || b == B) && m == b + 1);
}

static bool
allowed_htr(Move m)
{
	Move b = base_move(m);

	return moveset_HTM.allowed(m) && m == b + 1;
}

static bool
allowed_next_HTM(Move l2, Move l1, Move m)
{
	bool p, q;

	p = l1 != NULLMOVE && base_move(l1) == base_move(m);
	q = l2 != NULLMOVE && base_move(l2) == base_move(m);

	return !(p || (commute(l1, l2) && q));
}

void
append_alg(AlgList *l, Alg *alg)
{
	AlgListNode *node = malloc(sizeof(AlgListNode));
	int i;

	node->alg = new_alg("");
	for (i = 0; i < alg->len; i++)
		append_move(node->alg, alg->move[i], alg->inv[i]);
	node->next = NULL;

	if (++l->len == 1)
		l->first = node;
	else
		l->last->next = node;
	l->last = node;
}

void
append_move(Alg *alg, Move m, bool inverse)
{
	if (alg->len == alg->allocated)
		realloc_alg(alg, 2*alg->len);

	alg->move[alg->len] = m;
	alg->inv [alg->len] = inverse;
	alg->len++;
}

static int
axis(Move m)
{
	if (m == NULLMOVE)
		return 0;

	if (m >= U && m <= B3)
		return (m-1)/6 + 1;
	
	if (m >= Uw && m <= Bw3)
		return (m-1)/6 - 2;

	if (base_move(m) == E || base_move(m) == y)
		return 1;

	if (base_move(m) == M || base_move(m) == x)
		return 2;

	if (base_move(m) == S || base_move(m) == z)
		return 3;

	return -1;
}

Move
base_move(Move m)
{
	if (m == NULLMOVE)
		return NULLMOVE;
	else
		return m - (m-1)%3;
}

bool
commute(Move m1, Move m2)
{
	return axis(m1) == axis(m2);
}

void
compose_alg(Alg *alg1, Alg *alg2)
{
	int i;

	for (i = 0; i < alg2->len; i++)
		append_move(alg1, alg2->move[i], alg2->inv[i]);
}

void
copy_alg(Alg *src, Alg *dst)
{
	dst->len = 0; /* Overwrites */
	compose_alg(dst, src);
}

void
free_alg(Alg *alg)
{
	free(alg->move);
	free(alg->inv);
	free(alg);
}

void
free_alglist(AlgList *l)
{
	AlgListNode *aux, *i = l->first;

	while (i != NULL) {
		aux = i->next;
		free_alglistnode(i);
		i = aux;
	}
	free(l);
}

static void
free_alglistnode(AlgListNode *aln)
{
	free_alg(aln->alg);
	free(aln);
}

void
inplace(Alg * (*f)(Alg *), Alg *alg)
{
	Alg *aux;

	aux = f(alg);
	copy_alg(aux, alg);
	free(aux);
}

Alg *
inverse_alg(Alg *alg)
{
	Alg *ret = new_alg("");
	int i;

	for (i = alg->len-1; i >= 0; i--)
		append_move(ret, inverse_move(alg->move[i]), alg->inv[i]);

	return ret;
}

Move
inverse_move(Move m)
{
	return m == NULLMOVE ? NULLMOVE : m + 2 - 2*((m-1) % 3);
}

char *
move_string(Move m)
{
	static char move_string_aux[NMOVES][7] = {
		[NULLMOVE] = "-",
		[U]  = "U",  [U2]  = "U2",  [U3]  = "U\'",
		[D]  = "D",  [D2]  = "D2",  [D3]  = "D\'",
		[R]  = "R",  [R2]  = "R2",  [R3]  = "R\'",
		[L]  = "L",  [L2]  = "L2",  [L3]  = "L\'",
		[F]  = "F",  [F2]  = "F2",  [F3]  = "F\'",
		[B]  = "B",  [B2]  = "B2",  [B3]  = "B\'",
		[Uw] = "Uw", [Uw2] = "Uw2", [Uw3] = "Uw\'",
		[Dw] = "Dw", [Dw2] = "Dw2", [Dw3] = "Dw\'",
		[Rw] = "Rw", [Rw2] = "Rw2", [Rw3] = "Rw\'",
		[Lw] = "Lw", [Lw2] = "Lw2", [Lw3] = "Lw\'",
		[Fw] = "Fw", [Fw2] = "Fw2", [Fw3] = "Fw\'",
		[Bw] = "Bw", [Bw2] = "Bw2", [Bw3] = "Bw\'",
		[M]  = "M",  [M2]  = "M2",  [M3]  = "M\'",
		[E]  = "E",  [E2]  = "E2",  [E3]  = "E\'",
		[S]  = "S",  [S2]  = "S2",  [S3]  = "S\'",
		[x]  = "x",  [x2]  = "x2",  [x3]  = "x\'",
		[y]  = "y",  [y2]  = "y2",  [y3]  = "y\'",
		[z]  = "z",  [z2]  = "z2",  [z3]  = "z\'",
	};

	return move_string_aux[m];
}

Alg *
new_alg(char *str)
{
	Alg *alg;
	int i;
	bool niss, move_read;
	Move j, m;

	alg = malloc(sizeof(Alg));
	alg->move      = malloc(30 * sizeof(Move));
	alg->inv       = malloc(30 * sizeof(bool));
	alg->allocated = 30;
	alg->len       = 0;

	niss = false;
	for (i = 0; str[i]; i++) {
		if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
			continue;

		if (str[i] == '(' && niss) {
			fprintf(stderr, "Error reading moves: nested ( )\n");
			alg->len = 0;
			return alg;
		}

		if (str[i] == ')' && !niss) {
			fprintf(stderr, "Error reading moves: unmatched )\n");
			alg->len = 0;
			return alg;
		}

		if (str[i] == '(' || str[i] == ')') {
			niss = !niss;
			continue;
		}

		/* Single slash for comments */
		if (str[i] == '/') {
			while (str[i] && str[i] != '\n')
				i++;

			if (!str[i])
				i--;

			continue;
		}

		move_read = false;
		for (j = U; j < NMOVES; j++) {
			if (str[i] == move_string(j)[0] ||
			    (str[i] >= 'a' && str[i] <= 'z' &&
			     str[i] == move_string(j)[0]-('A'-'a') && j<=B)) {
				m = j;
				if (str[i] >= 'a' && str[i] <= 'z' && j<=B) {
					m += Uw - U;
				}
				if (m <= B && str[i+1]=='w') {
					m += Uw - U;
					i++;
				}
				if (str[i+1]=='2') {
					m += 1;
					i++;
				} else if (str[i+1] == '\'' ||
				           str[i+1] == '3'  ||
					   str[i+1] == '`' ) {
					m += 2;
					i++;
				} else if ((int)str[i+1] == -62 &&
					   (int)str[i+2] == -76) {
					/* Weird apostrophe */
					m += 2;
					i += 2;
				} else if ((int)str[i+1] == -30 &&
					   (int)str[i+2] == -128 &&
					   (int)str[i+3] == -103) {
					/* MacOS apostrophe */
					m += 2;
					i += 3;
				}
				append_move(alg, m, niss);
				move_read = true;
				break;
			}
		}

		if (!move_read) {
			free(alg);
			return new_alg("");
		}
	}

	if (niss) {
		fprintf(stderr, "Error reading moves: unmatched (\n");
		alg->len = 0;
	}

	return alg;
}

AlgList *
new_alglist(void)
{
	AlgList *ret = malloc(sizeof(AlgList));

	ret->len   = 0;
	ret->first = NULL;
	ret->last  = NULL;

	return ret;
}

Alg *
on_inverse(Alg *alg)
{
	Alg *ret = new_alg("");
	int i;

	for (i = 0; i < alg->len; i++)
		append_move(ret, alg->move[i], !alg->inv[i]);

	return ret;
}

void
print_alg(FILE *out, Alg *alg, bool l)
{
	char fill[4];
	int i;
	bool niss = false;

	for (i = 0; i < alg->len; i++) {
		if (!niss && alg->inv[i])
			strcpy(fill, i == 0 ? "(" : " (");
		if (niss && !alg->inv[i])
			strcpy(fill, ") ");
		if (niss == alg->inv[i])
			strcpy(fill, i == 0 ? "" : " ");

		fprintf(out, "%s%s", fill, move_string(alg->move[i]));
		niss = alg->inv[i];
	}

	if (niss)
		fprintf(out, ")");
	if (l)
		fprintf(out, " (%d)", alg->len);

	fprintf(out, "\n");
}

void
print_alglist(FILE *out, AlgList *al, bool l)
{
	AlgListNode *i;

	for (i = al->first; i != NULL; i = i->next)
		print_alg(out, i->alg, l);
}

static void
realloc_alg(Alg *alg, int n)
{
	if (alg == NULL) {
		fprintf(stderr, "Error: trying to reallocate NULL alg.\n");
		return;
	}

	if (n < alg->len) {
		fprintf(stderr, "Error: alg too long for reallocation ");
		fprintf(stderr, "(%d vs %d)\n", alg->len, n);
		return;
	}

	if (n > 1000000) {
		fprintf(stderr, "Warning: very long alg,");
		fprintf(stderr, "something might go wrong.\n");
	}

	alg->move = realloc(alg->move, n * sizeof(int));
	alg->inv  = realloc(alg->inv,  n * sizeof(int));
	alg->allocated = n;
}

static int
niss_type(Alg *a)
{
	/* 0 if all moves are on normal, 1 if all on inverse, 2 otherwise */

	int i;
	bool found_normal = false, found_inverse = false;

	for (i = 0; i < a->len; i++) {
		found_normal = found_normal || !a->inv[i];
		found_inverse = found_inverse || a->inv[i];
	}

	if (found_normal && !found_inverse)
		return 0;
	if (!found_normal && found_inverse)
		return 1;
	return 2;
}

static void
find_last_moves(Alg *a, bool inv, int *n, int *nlast, int *nslast)
{
	int i;

	for (i = 0, *n = 0, *nlast = -1, *nslast = -1; i < a->len; i++) {
		if (inv == a->inv[i]) {
			(*n)++;
			*nslast = *nlast;
			*nlast = i;
		}
	}
}

static int64_t
last_move_pair(Alg *a, bool inv)
{
	/* Order: _ F*, _ B*, F* B*, ... U* D* */

	static int bit[] = {
		[F] = 16, [B] = 16,
		[R] = 18, [L] = 18,
		[U] = 20, [D] = 20 
	};
	int n, nlast, nslast, last, slast;

	find_last_moves(a, inv, &n, &nlast, &nslast);

	if (n == 0)
		return -1;

	last = a->move[nlast];
	slast = (nslast != -1 && commute(a->move[nlast], a->move[nslast])) ?
	    a->move[nslast] : 0;

	return (slast * NMOVES + last) + (1LL << bit[base_move(last)]);
}

static int
compare_algs_firstmoves(Alg *a, Alg *b, bool inv)
{
	/* Compare algs up to the last or the last two moves if parallel */

	int i, j, na, nlasta, nslasta, nb, nlastb, nslastb, ma, mb, m1, m2;

	find_last_moves(a, inv, &na, &nlasta, &nslasta);
	find_last_moves(b, inv, &nb, &nlastb, &nslastb);

	ma = na > 0 ? ((na > 1 ? nslasta : nlasta) + 1) : 0;
	mb = nb > 0 ? ((nb > 1 ? nslastb : nlastb) + 1) : 0;

	if (ma == 0 && mb == 0)
		return 0;
	if (ma == 0)
		return -1;
	if (mb == 0)
		return 1;

	for (i = 0, j = 0; i < ma && j < mb; i++, j++) {
		while (a->inv[i] != inv) i++;
		while (a->inv[j] != inv) j++;
		m1 = a->move[i];
		m2 = b->move[j];
		if (m1 - m2)
			return m1 - m2;
	}

	return ma - mb;
}

static int
compare_algs(const void *avoid, const void *bvoid)
{
	/* We sort a list of algs in a way that makes the most sense for the
	 * commands and steps where one usually wants many results, for
	 * example EO and DR. We sort by, in order:
	 *   1. Length of the solution
	 *   2. NISS type (first algs that are all on normal, then those that
	 *      are all on inverse, then those that use NISS)
	 *   3. Last move on normal, or last two moves if they are parallel
	 *   4. All other moves on normal scramble
	 *   5. Last move on inverse, or last two moves if they are parallel
	 *   6. All other moves on inverse
	 */

	Alg *a = *(Alg **)avoid;
	Alg *b = *(Alg **)bvoid;

	int ntype_a, ntype_b, last_a, last_b, cmp;

	/* 1. Compare length */
	if (a->len - b->len)
		return a->len - b->len;

	/* 2. Algs have the same length, compare NISS type */
	ntype_a = niss_type(a);
	ntype_b = niss_type(b);
	if (ntype_a - ntype_b)
		return ntype_a - ntype_b;

	/* 3. Algs have same NISS type, compare last moves on normal */
	last_a = last_move_pair(a, false);
	last_b = last_move_pair(b, false);
	if (last_a - last_b)
		return last_a - last_b;

	/* 4. Algs have same last moves on normal, compare all other moves */
	cmp = compare_algs_firstmoves(a, b, false);
	if (cmp)
		return cmp;

	/* 5. Algs have same moves on normal, compare last on inverse */
	last_a = last_move_pair(a, true);
	last_b = last_move_pair(b, true);
	if (last_a - last_b)
		return last_a - last_b;

	/* 6. Algs have same last moves on inverse, compare other */
	cmp = compare_algs_firstmoves(a, b, true);
	if (cmp)
		return cmp;

	/* Algs are equal */
	return 0;
}

void
sort_alglist(AlgList *al)
{
	int i, n = al->len;
	Alg* alg_array[n+1];
	AlgListNode *node;

	for (i = 0, node = al->first; i < n; i++, node = node->next)
		alg_array[i] = node->alg;

	qsort(alg_array, n, sizeof(Alg *), &compare_algs);

	for (i = 0, node = al->first; i < n; i++, node = node->next)
		node->alg = alg_array[i];
}

void
swapmove(Move *m1, Move *m2)
{
	Move aux;

	aux = *m1;
	*m1 = *m2;
	*m2 = aux;
}

Alg *
unniss(Alg *alg)
{
	int i;
	Alg *ret;

	ret = new_alg("");

	for (i = 0; i < alg->len; i++)
		if (!alg->inv[i])
			append_move(ret, alg->move[i], false);
	
	for (i = alg->len-1; i >= 0; i--)
		if (alg->inv[i])
			append_move(ret, inverse_move(alg->move[i]), false);
	
	return ret;
}

void
init_moveset(Moveset *ms)
{
	int j;
	uint64_t l, one;
	Move m, l2, l1;

	one = 1;

	for (j = 0, m = U; m < NMOVES; m++)
		if (ms->allowed(m))
			ms->sorted_moves[j++] = m;
	ms->sorted_moves[j] = NULLMOVE;

	for (l1 = 0; l1 < NMOVES; l1++) { 
		for (l2 = 0; l2 < NMOVES; l2++) { 
			ms->mask[l2][l1] = 0;
			for (l=0; ms->sorted_moves[l]!=NULLMOVE; l++) {
				m = ms->sorted_moves[l];
				if (ms->allowed_next(l2, l1, m))
					ms->mask[l2][l1] |= (one<<m);
			}
		}
	}
}

void
init_all_movesets(void)
{
	int i;

	for (i = 0; i < nmoveset; i++)
		init_moveset(all_ms[i]);
}
