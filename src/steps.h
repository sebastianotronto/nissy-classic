#ifndef STEPS_H
#define STEPS_H

#include "pruning.h"

extern Step *           steps[];

/* Two steps used directly by two-phase solver */
extern Step             drany_HTM;
extern Step             dranyfin_DR;
/* Step used directly by the scramble command */
extern Step             drudfin_drud;

void                    copy_estimatedata(EstimateData *s, EstimateData *d);
void                    invert_estimatedata(EstimateData *ed);
void                    reset_estimatedata(EstimateData *ed);
void                    prepare_step(Step *step, SolveOptions *opts);

#endif
