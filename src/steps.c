#include "steps.h"

/* Checkers, estimators and validators ***************************************/

static bool             check_centers(Cube cube);
static bool             check_eofb(Cube cube);
static bool             check_drud(Cube cube);
static bool             check_htr(Cube cube);

static int              estimate_eoany_HTM(CubeTarget ct);
static int              estimate_eofb_HTM(CubeTarget ct);
static int              estimate_coany_HTM(CubeTarget ct);
static int              estimate_coud_HTM(CubeTarget ct);
static int              estimate_coany_URF(CubeTarget ct);
static int              estimate_coud_URF(CubeTarget ct);
static int              estimate_corners_HTM(CubeTarget ct);
static int              estimate_cornershtr_HTM(CubeTarget ct);
static int              estimate_corners_URF(CubeTarget ct);
static int              estimate_cornershtr_URF(CubeTarget ct);
static int              estimate_drany_HTM(CubeTarget ct);
static int              estimate_drud_HTM(CubeTarget ct);
static int              estimate_drud_eofb(CubeTarget ct);
static int              estimate_dr_eofb(CubeTarget ct);
static int              estimate_drudfin_drud(CubeTarget ct);
static int              estimate_htr_drud(CubeTarget ct);
static int              estimate_htrfin_htr(CubeTarget ct);
static int              estimate_optimal_HTM(CubeTarget ct);

static bool             always_valid(Alg *alg);
static bool             validate_singlecw_ending(Alg *alg);

/* Pre-transformation detectors **********************************************/

static Trans            detect_pretrans_eofb(Cube cube);
static Trans            detect_pretrans_drud(Cube cube);

/* Messages for when cube is not ready ***************************************/

static char check_centers_msg[100] = "cube must be oriented (centers solved)";
static char check_eo_msg[100]      = "EO must be solved on given axis";
static char check_dr_msg[100]      = "DR must be solved on given axis";
static char check_htr_msg[100]     = "HTR must be solved";
static char check_drany_msg[100]   = "DR must be solved on at least one axis";

/* Steps *********************************************************************/

Step
optimal_HTM = {
	.shortname = "optimal",
	.name      = "Optimal solve (in HTM)",

	.estimate  = estimate_optimal_HTM,
	.ready     = check_centers,
	.ready_msg = check_centers_msg,
	.is_valid  = always_valid,
	.moveset   = moveset_HTM,

	.pre_trans = uf,

	.tables    = {&pd_khuge_HTM, &pd_corners_HTM},
	.ntables   = 2,
};

/* EO steps **************************/
Step
eoany_HTM = {
	.shortname = "eo",
	.name      = "EO on any axis",

	.estimate  = estimate_eoany_HTM,
	.ready     = check_centers,
	.ready_msg = check_centers_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_HTM,

	.pre_trans = uf,

	.tables    = {&pd_eofb_HTM},
	.ntables   = 1,
};

Step
eofb_HTM = {
	.shortname = "eofb",
	.name      = "EO on F/B",

	.estimate  = estimate_eofb_HTM,
	.ready     = check_centers,
	.ready_msg = check_centers_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_HTM,

	.pre_trans = uf,

	.tables    = {&pd_eofb_HTM},
	.ntables   = 1,
};

Step
eorl_HTM = {
	.shortname = "eorl",
	.name      = "EO on R/L",

	.estimate  = estimate_eofb_HTM,
	.ready     = check_centers,
	.ready_msg = check_centers_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_HTM,

	.pre_trans = ur,

	.tables    = {&pd_eofb_HTM},
	.ntables   = 1,
};

Step
eoud_HTM = {
	.shortname = "eoud",
	.name      = "EO on U/D",

	.estimate  = estimate_eofb_HTM,
	.ready     = check_centers,
	.ready_msg = check_centers_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_HTM,

	.pre_trans = fd,

	.tables    = {&pd_eofb_HTM},
	.ntables   = 1,
};

/* CO steps **************************/
Step
coany_HTM = {
	.shortname = "co",
	.name      = "CO on any axis",

	.estimate  = estimate_coany_HTM,
	.ready     = NULL,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_HTM,

	.pre_trans = uf,

	.tables    = {&pd_coud_HTM},
	.ntables   = 1,
};

Step
coud_HTM = {
	.shortname = "coud",
	.name      = "CO on U/D",

	.estimate  = estimate_coud_HTM,
	.ready     = NULL,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_HTM,

	.pre_trans = uf,

	.tables    = {&pd_coud_HTM},
	.ntables   = 1,
};

Step
corl_HTM = {
	.shortname = "corl",
	.name      = "CO on R/L",

	.estimate  = estimate_coud_HTM,
	.ready     = NULL,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_HTM,

	.pre_trans = rf,

	.tables    = {&pd_coud_HTM},
	.ntables   = 1,
};

Step
cofb_HTM = {
	.shortname = "cofb",
	.name      = "CO on F/B",

	.estimate  = estimate_coud_HTM,
	.ready     = NULL,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_HTM,

	.pre_trans = fd,

	.tables    = {&pd_coud_HTM},
	.ntables   = 1,
};

Step
coany_URF = {
	.shortname = "co-URF",
	.name      = "CO any axis (URF moveset)",

	.estimate  = estimate_coany_URF,
	.ready     = NULL,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_URF,

	.pre_trans = uf,

	.tables    = {&pd_coud_HTM},
	.ntables   = 1,
};

Step
coud_URF = {
	.shortname = "coud-URF",
	.name      = "CO on U/D (URF moveset)",

	.estimate  = estimate_coud_URF,
	.ready     = NULL,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_URF,

	.pre_trans = uf,

	.tables    = {&pd_coud_HTM},
	.ntables   = 1,
};

Step
corl_URF = {
	.shortname = "corl-URF",
	.name      = "CO on R/L (URF moveset)",

	.estimate  = estimate_coud_URF,
	.ready     = NULL,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_URF,

	.pre_trans = rf,

	.tables    = {&pd_coud_HTM},
	.ntables   = 1,
};

Step
cofb_URF = {
	.shortname = "cofb-URF",
	.name      = "CO on F/B (URF moveset)",

	.estimate  = estimate_coud_URF,
	.ready     = NULL,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_URF,

	.pre_trans = fd,

	.tables    = {&pd_coud_HTM},
	.ntables   = 1,
};

/* Misc corner steps *****************/
Step
cornershtr_HTM = {
	.shortname = "chtr",
	.name      = "Solve corners to HTR state",

	.estimate  = estimate_cornershtr_HTM,
	.ready     = NULL,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_HTM,

	.pre_trans = uf,

	.tables    = {&pd_cornershtr_HTM},
	.ntables   = 1,
};

Step
cornershtr_URF = {
	.shortname = "chtr-URF",
	.name      = "Solve corners to HTR state (URF moveset)",

	.estimate  = estimate_cornershtr_URF,
	.ready     = NULL,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_URF,

	.pre_trans = uf,

	.tables    = {&pd_cornershtr_HTM},
	.ntables   = 1,
};

Step
corners_HTM = {
	.shortname = "corners",
	.name      = "Solve corners",

	.estimate  = estimate_corners_HTM,
	.ready     = NULL,
	.is_valid  = always_valid,
	.moveset   = moveset_HTM,

	.pre_trans = uf,

	.tables    = {&pd_corners_HTM},
	.ntables   = 1,
};

Step
corners_URF = {
	.shortname = "corners-URF",
	.name      = "Solve corners (URF moveset)",

	.estimate  = estimate_corners_URF,
	.ready     = NULL,
	.is_valid  = always_valid,
	.moveset   = moveset_URF,

	.pre_trans = uf,

	.tables    = {&pd_corners_HTM},
	.ntables   = 1,
};

/* DR steps **************************/
Step
drany_HTM = {
	.shortname = "dr",
	.name      = "DR on any axis",

	.estimate  = estimate_drany_HTM,
	.ready     = check_centers,
	.ready_msg = check_centers_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_HTM,

	.pre_trans = uf,

	.tables    = {&pd_drud_sym16_HTM},
	.ntables   = 1,
};

Step
drud_HTM = {
	.shortname = "drud",
	.name      = "DR on U/D",

	.estimate  = estimate_drud_HTM,
	.ready     = check_centers,
	.ready_msg = check_centers_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_HTM,

	.pre_trans = uf,

	.tables    = {&pd_drud_sym16_HTM},
	.ntables   = 1,
};

Step
drrl_HTM = {
	.shortname = "drrl",
	.name      = "DR on R/L",

	.estimate  = estimate_drud_HTM,
	.ready     = check_centers,
	.ready_msg = check_centers_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_HTM,

	.pre_trans = rf,

	.tables    = {&pd_drud_sym16_HTM},
	.ntables   = 1,
};

Step
drfb_HTM = {
	.shortname = "drfb",
	.name      = "DR on F/B",

	.estimate  = estimate_drud_HTM,
	.ready     = check_centers,
	.ready_msg = check_centers_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_HTM,

	.pre_trans = fd,

	.tables    = {&pd_drud_sym16_HTM},
	.ntables   = 1,
};

/* DR from EO */
Step
dr_eo = {
	.shortname = "dr-eo",
	.name      = "DR without breaking EO (automatically detected)",

	.estimate  = estimate_dr_eofb,
	.ready     = check_eofb,
	.ready_msg = check_eo_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_eofb,

	.detect    = detect_pretrans_eofb,

	.tables    = {&pd_drud_eofb},
	.ntables   = 1,
};

Step
dr_eofb = {
	.shortname = "dr-eofb",
	.name      = "DR on U/D or R/L without breaking EO on F/B",

	.estimate  = estimate_dr_eofb,
	.ready     = check_eofb,
	.ready_msg = check_eo_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_eofb,

	.pre_trans = uf,

	.tables    = {&pd_drud_eofb},
	.ntables   = 1,
};

Step
dr_eorl = {
	.shortname = "dr-eorl",
	.name      = "DR on U/D or F/B without breaking EO on R/L",

	.estimate  = estimate_dr_eofb,
	.ready     = check_eofb,
	.ready_msg = check_eo_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_eofb,

	.pre_trans = ur,

	.tables    = {&pd_drud_eofb},
	.ntables   = 1,
};

Step
dr_eoud = {
	.shortname = "dr-eoud",
	.name      = "DR on R/L or F/B without breaking EO on U/D",

	.estimate  = estimate_dr_eofb,
	.ready     = check_eofb,
	.ready_msg = check_eo_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_eofb,

	.pre_trans = fd,

	.tables    = {&pd_drud_eofb},
	.ntables   = 1,
};

Step
drud_eofb = {
	.shortname = "drud-eofb",
	.name      = "DR on U/D without breaking EO on F/B",

	.estimate  = estimate_drud_eofb,
	.ready     = check_eofb,
	.ready_msg = check_eo_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_eofb,

	.pre_trans = uf,

	.tables    = {&pd_drud_eofb},
	.ntables   = 1,
};

Step
drrl_eofb = {
	.shortname = "drrl-eofb",
	.name      = "DR on R/L without breaking EO on F/B",

	.estimate  = estimate_drud_eofb,
	.ready     = check_eofb,
	.ready_msg = check_eo_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_eofb,

	.pre_trans = rf,

	.tables    = {&pd_drud_eofb},
	.ntables   = 1,
};

Step
drud_eorl = {
	.shortname = "drud-eorl",
	.name      = "DR on U/D without breaking EO on R/L",

	.estimate  = estimate_drud_eofb,
	.ready     = check_eofb,
	.ready_msg = check_eo_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_eofb,

	.pre_trans = ur,

	.tables    = {&pd_drud_eofb},
	.ntables   = 1,
};

Step
drfb_eorl = {
	.shortname = "drfb-eorl",
	.name      = "DR on F/B without breaking EO on R/L",

	.estimate  = estimate_drud_eofb,
	.ready     = check_eofb,
	.ready_msg = check_eo_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_eofb,

	.pre_trans = fr,

	.tables    = {&pd_drud_eofb},
	.ntables   = 1,
};

Step
drfb_eoud = {
	.shortname = "drfb-eoud",
	.name      = "DR on F/B without breaking EO on U/D",

	.estimate  = estimate_drud_eofb,
	.ready     = check_eofb,
	.ready_msg = check_eo_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_eofb,

	.pre_trans = fd,

	.tables    = {&pd_drud_eofb},
	.ntables   = 1,
};

Step
drrl_eoud = {
	.shortname = "drrl-eoud",
	.name      = "DR on R/L without breaking EO on U/D",

	.estimate  = estimate_drud_eofb,
	.ready     = check_eofb,
	.ready_msg = check_eo_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_eofb,

	.pre_trans = rd,

	.tables    = {&pd_drud_eofb},
	.ntables   = 1,
};

/* DR finish steps */
Step
dranyfin_DR = {
	.shortname = "drfin",
	.name      = "DR finish on any axis without breaking DR",

	.estimate  = estimate_drudfin_drud,
	.ready     = check_drud,
	.ready_msg = check_drany_msg,
	.is_valid  = always_valid,
	.moveset   = moveset_drud,

	.detect    = detect_pretrans_drud,

	.tables    = {&pd_drudfin_noE_sym16_drud},
	.ntables   = 1,
};

Step
drudfin_drud = {
	.shortname = "drudfin",
	.name      = "DR finish on U/D without breaking DR",

	.estimate  = estimate_drudfin_drud,
	.ready     = check_drud,
	.ready_msg = check_dr_msg,
	.is_valid  = always_valid,
	.moveset   = moveset_drud,

	.pre_trans = uf,

	.tables    = {&pd_drudfin_noE_sym16_drud},
	.ntables   = 1,
};

Step
drrlfin_drrl = {
	.shortname = "drrlfin",
	.name      = "DR finish on R/L without breaking DR",

	.estimate  = estimate_drudfin_drud,
	.ready     = check_drud,
	.ready_msg = check_dr_msg,
	.is_valid  = always_valid,
	.moveset   = moveset_drud,

	.pre_trans = rf,

	.tables    = {&pd_drudfin_noE_sym16_drud},
	.ntables   = 1,
};

Step
drfbfin_drfb = {
	.shortname = "drfbfin",
	.name      = "DR finish on F/B without breaking DR",

	.estimate  = estimate_drudfin_drud,
	.ready     = check_drud,
	.ready_msg = check_dr_msg,
	.is_valid  = always_valid,
	.moveset   = moveset_drud,

	.pre_trans = fd,

	.tables    = {&pd_drudfin_noE_sym16_drud},
	.ntables   = 1,
};

/* HTR from DR */
Step
htr_any = {
	.shortname = "htr",
	.name      = "HTR from DR",

	.estimate  = estimate_htr_drud,
	.ready     = check_drud,
	.ready_msg = check_drany_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_drud,

	.detect    = detect_pretrans_drud,

	.tables    = {&pd_htr_drud},
	.ntables   = 1,
};

Step
htr_drud = {
	.shortname = "htr-drud",
	.name      = "HTR from DR on U/D",

	.estimate  = estimate_htr_drud,
	.ready     = check_drud,
	.ready_msg = check_dr_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_drud,

	.pre_trans = uf,

	.tables    = {&pd_htr_drud},
	.ntables   = 1,
};

Step
htr_drrl = {
	.shortname = "htr-drrl",
	.name      = "HTR from DR on R/L",

	.estimate  = estimate_htr_drud,
	.ready     = check_drud,
	.ready_msg = check_dr_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_drud,

	.pre_trans = rf,

	.tables    = {&pd_htr_drud},
	.ntables   = 1,
};

Step
htr_drfb = {
	.shortname = "htr-drfb",
	.name      = "HTR from DR on F/B",

	.estimate  = estimate_htr_drud,
	.ready     = check_drud,
	.ready_msg = check_dr_msg,
	.is_valid  = validate_singlecw_ending,
	.moveset   = moveset_drud,

	.pre_trans = fd,

	.tables    = {&pd_htr_drud},
	.ntables   = 1,
};

/* HTR finish */
Step
htrfin_htr = {
	.shortname = "htrfin",
	.name      = "HTR finish without breaking HTR",

	.estimate  = estimate_htrfin_htr,
	.ready     = check_htr,
	.ready_msg = check_htr_msg,
	.is_valid  = always_valid,
	.moveset   = moveset_htr,

	.pre_trans = uf,

	.tables    = {&pd_htrfin_htr},
	.ntables   = 1,
};

Step *steps[NSTEPS] = {
	&optimal_HTM, /* first is default */

	&eoany_HTM,
	&eofb_HTM,
	&eorl_HTM,
	&eoud_HTM,

	&coany_HTM,
	&coud_HTM,
	&corl_HTM,
	&cofb_HTM,

	&coany_URF,
	&coud_URF,
	&corl_URF,
	&cofb_URF,

	&drany_HTM,
	&drud_HTM,
	&drrl_HTM,
	&drfb_HTM,

	&dr_eo,
	&dr_eofb,
	&dr_eorl,
	&dr_eoud,
	&drud_eofb,
	&drrl_eofb,
	&drud_eorl,
	&drfb_eorl,
	&drfb_eoud,
	&drrl_eoud,

	&dranyfin_DR,
	&drudfin_drud,
	&drrlfin_drrl,
	&drfbfin_drfb,

	&htr_any,
	&htr_drud,
	&htr_drrl,
	&htr_drfb,

	&htrfin_htr,

	&cornershtr_HTM,
	&cornershtr_URF,
	&corners_HTM,
	&corners_URF,
};

/* Checkers, estimators and validators ***************************************/

static bool
check_centers(Cube cube)
{
	return cube.cpos == 0;
}

static bool
check_eofb(Cube cube)
{
	return cube.eofb == 0;
}

static bool
check_drud(Cube cube)
{
	return cube.eofb == 0 && cube.eorl == 0 && cube.coud == 0;
}

static bool
check_htr(Cube cube)
{
	return check_drud(cube) && coord_htr_drud.index(cube) == 0;
}

static int
estimate_eoany_HTM(CubeTarget ct)
{
	int r1, r2, r3;

	r1 = ptableval(&pd_eofb_HTM, ct.cube);
	r2 = ptableval(&pd_eofb_HTM, apply_trans(ur, ct.cube));
	r3 = ptableval(&pd_eofb_HTM, apply_trans(fd, ct.cube));

	return MIN(r1, MIN(r2, r3));
}

static int
estimate_eofb_HTM(CubeTarget ct)
{
	return ptableval(&pd_eofb_HTM, ct.cube);
}

static int
estimate_coany_HTM(CubeTarget ct)
{
	int r1, r2, r3;

	r1 = ptableval(&pd_coud_HTM, ct.cube);
	r2 = ptableval(&pd_coud_HTM, apply_trans(rf, ct.cube));
	r3 = ptableval(&pd_coud_HTM, apply_trans(fd, ct.cube));

	return MIN(r1, MIN(r2, r3));
}

static int
estimate_coud_HTM(CubeTarget ct)
{
	return ptableval(&pd_coud_HTM, ct.cube);
}

static int
estimate_coany_URF(CubeTarget ct)
{
	int r1, r2, r3;
	CubeTarget ct2, ct3;

	ct2.cube = apply_trans(rf, ct.cube);
	ct2.target = ct.target;

	ct3.cube = apply_trans(fd, ct.cube);
	ct3.target = ct.target;

	r1 = estimate_coud_URF(ct);
	r2 = estimate_coud_URF(ct2);
	r3 = estimate_coud_URF(ct3);

	return MIN(r1, MIN(r2, r3));
}

static int
estimate_coud_URF(CubeTarget ct)
{
	/* TODO: I can improve this by checking first the orientation of
	 * the corner in DBL and use that as a reference */

	CubeTarget ct2 = {.cube = apply_move(z, ct.cube), .target = ct.target};
	CubeTarget ct3 = {.cube = apply_move(x, ct.cube), .target = ct.target};

	int ud = estimate_coud_HTM(ct);
	int rl = estimate_coud_HTM(ct2);
	int fb = estimate_coud_HTM(ct3);

	return MIN(ud, MIN(rl, fb));
}

static int
estimate_corners_HTM(CubeTarget ct)
{
	return ptableval(&pd_corners_HTM, ct.cube);
}

static int
estimate_cornershtr_HTM(CubeTarget ct)
{
	return ptableval(&pd_cornershtr_HTM, ct.cube);
}

static int
estimate_cornershtr_URF(CubeTarget ct)
{
	/* TODO: I can improve this by checking first the corner in DBL
	 * and use that as a reference */

	int c, ret = 15;
	Trans i;

	for (i = 0; i < NROTATIONS; i++) {
		ct.cube = apply_alg(rotation_alg(i), ct.cube);
		c = estimate_cornershtr_HTM(ct);
		ret = MIN(ret, c);
	}

	return ret;
}

static int
estimate_corners_URF(CubeTarget ct)
{
	/* TODO: I can improve this by checking first the corner in DBL
	 * and use that as a reference */

	int c, ret = 15;
	Trans i;

	for (i = 0; i < NROTATIONS; i++) {
		ct.cube = apply_alg(rotation_alg(i), ct.cube);
		c = estimate_corners_HTM(ct);
		ret = MIN(ret, c);
	}

	return ret;
}

static int
estimate_drany_HTM(CubeTarget ct)
{
	int r1, r2, r3;

	r1 = ptableval(&pd_drud_sym16_HTM, ct.cube);
	r2 = ptableval(&pd_drud_sym16_HTM, apply_trans(rf, ct.cube));
	r3 = ptableval(&pd_drud_sym16_HTM, apply_trans(fd, ct.cube));

	return MIN(r1, MIN(r2, r3));
}

static int
estimate_drud_HTM(CubeTarget ct)
{
	return ptableval(&pd_drud_sym16_HTM, ct.cube);
}

static int
estimate_drud_eofb(CubeTarget ct)
{
	return ptableval(&pd_drud_eofb, ct.cube);
}

static int
estimate_dr_eofb(CubeTarget ct)
{
	int r1, r2;

	r1 = ptableval(&pd_drud_eofb, ct.cube);
	r2 = ptableval(&pd_drud_eofb, apply_trans(rf, ct.cube));

	return MIN(r1, r2);
}

static int
estimate_drudfin_drud(CubeTarget ct)
{
	int val = ptableval(&pd_drudfin_noE_sym16_drud, ct.cube);

	if (val != 0)
		return val;

	return ct.cube.epose % 24 == 0 ? 0 : 1;
}

static int
estimate_htr_drud(CubeTarget ct)
{
	return ptableval(&pd_htr_drud, ct.cube);
}

static int
estimate_htrfin_htr(CubeTarget ct)
{
	return ptableval(&pd_htrfin_htr, ct.cube);
}

static int
estimate_optimal_HTM(CubeTarget ct)
{
	int dr1, dr2, dr3, cor, ret;
	Cube inv;

	dr1 = ptableval(&pd_khuge_HTM, ct.cube);
	cor = estimate_corners_HTM(ct);
	ret = MAX(dr1, cor);
	if (ret > ct.target)
		return ret;

	dr2 = ptableval(&pd_khuge_HTM, apply_trans(rf, ct.cube));
	ret = MAX(ret, dr2);
	if (ret > ct.target)
		return ret;

	dr3 = ptableval(&pd_khuge_HTM, apply_trans(fd, ct.cube));
	if (dr1 == dr2 && dr2 == dr3 && dr1 != 0)
		dr3++;
	ret = MAX(ret, dr3);
	if (ret > ct.target || ret == 0)
		return ret;

	/* Inverse cube probing */

	inv = inverse_cube(ct.cube);
	dr1 = ptableval(&pd_khuge_HTM, inv);
	ret = MAX(ret, dr1);
	if (ret > ct.target)
		return ret;

	dr2 = ptableval(&pd_khuge_HTM, apply_trans(rf, inv));
	ret = MAX(ret, dr2);
	if (ret > ct.target)
		return ret;
	
	dr3 = ptableval(&pd_khuge_HTM, apply_trans(fd, inv));
	if (dr1 == dr2 && dr2 == dr3 && dr1 != 0)
		dr3++;
	return MAX(ret, dr3);
}

static bool
always_valid(Alg *alg)
{
	return true;
}

static bool
validate_singlecw_ending(Alg *alg)
{
	int i;
	bool nor, inv;
	Move l2 = NULLMOVE, l1 = NULLMOVE, l2i = NULLMOVE, l1i = NULLMOVE;

	for (i = 0; i < alg->len; i++) {
		if (alg->inv[i]) {
			l2i = l1i;
			l1i = alg->move[i];
		} else {
			l2 = l1;
			l1 = alg->move[i];
		}
	}

	nor = l1 ==base_move(l1)  && (!commute(l1, l2) ||l2 ==base_move(l2));
	inv = l1i==base_move(l1i) && (!commute(l1i,l2i)||l2i==base_move(l2i));

	return nor && inv;
}

/* Pre-transformation detectors **********************************************/

static Trans
detect_pretrans_eofb(Cube cube)
{
	Trans i;

	for (i = 0; i < NROTATIONS; i++)
		if (check_eofb(apply_trans(i, cube)))
			return i;

	return 0;
}

static Trans
detect_pretrans_drud(Cube cube)
{
	Trans i;

	for (i = 0; i < NROTATIONS; i++)
		if (check_drud(apply_trans(i, cube)))
			return i;

	return 0;
}

/* Public functions **********************************************************/

void
prepare_step(Step *step)
{
	int i;

	for (i = 0; i < step->ntables; i++)
		genptable(step->tables[i]);
}
