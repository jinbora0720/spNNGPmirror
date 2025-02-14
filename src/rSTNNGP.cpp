#define USE_FC_LEN_T
#include <string>
#include <iostream>
#include "util.h"

#ifdef _OPENMP
#include <omp.h>
#endif 

#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include <R_ext/Linpack.h>
#include <R_ext/Lapack.h>
#include <R_ext/BLAS.h>
#ifndef FCONE
# define FCONE
#endif

double updateBF_parent_r(double *B, double *F, double *c, double *C, double *coords,
                         int *nnIndx, int *nnIndxLU,
                         int *nnIndxParent, int *nnIndxLUParent, int *nnIndxLUAll,
                         int n, int m, int twomp1, int twomp1sq,
                         double *theta, int sigmaSqIndx, int phiIndx, int nuIndx,
                         double tauSq, double tauSqParent, double rho, 
                         int covModel, double *bk, double nuUnifb, bool root) {
  
  int i, k, l;
  int info = 0;
  int inc = 1;
  double one = 1.0;
  double zero = 0.0;
  char lower = 'L';
  double logDet = 0;
  double nu = 0;
  
  if(getCorName(covModel) == "matern"){
    nu = theta[nuIndx];
  }
  
  //bk must be 1+(int)floor(alpha) * nthread
  int nb = 1+static_cast<int>(floor(nuUnifb));
  int threadID = 0;
  double e;
  
  if (root) {
#ifdef _OPENMP
#pragma omp parallel for private(k, l, info, threadID, e)
#endif
    for(i = 0; i < n; i++){
#ifdef _OPENMP
      threadID = omp_get_thread_num();
#endif
      if (i > 0) {
        for(k = 0; k < nnIndxLU[n+i]; k++){
          e = dist2(coords[i], coords[n+i], coords[nnIndx[nnIndxLU[i]+k]], coords[n+nnIndx[nnIndxLU[i]+k]]);
          c[twomp1*threadID+k] = theta[sigmaSqIndx]*spCor(e, theta[phiIndx], nu, covModel, &bk[threadID*nb]);
          for(l = 0; l <= k; l++){
            e = dist2(coords[nnIndx[nnIndxLU[i]+k]], coords[n+nnIndx[nnIndxLU[i]+k]], coords[nnIndx[nnIndxLU[i]+l]], coords[n+nnIndx[nnIndxLU[i]+l]]);  
            C[twomp1sq*threadID+l*nnIndxLU[n+i]+k] = theta[sigmaSqIndx]*spCor(e, theta[phiIndx], nu, covModel, &bk[threadID*nb]); 
            if(l == k){
              C[twomp1sq*threadID+l*nnIndxLU[n+i]+k] += tauSq;
            }
          }
        }
        F77_NAME(dpotrf)(&lower, &nnIndxLU[n+i], &C[twomp1sq*threadID], &nnIndxLU[n+i], &info FCONE);
        if(info != 0){error("c++ error: dpotrf failed\n");}
        F77_NAME(dpotri)(&lower, &nnIndxLU[n+i], &C[twomp1sq*threadID], &nnIndxLU[n+i], &info FCONE);
        F77_NAME(dsymv)(&lower, &nnIndxLU[n+i], &one, &C[twomp1sq*threadID], &nnIndxLU[n+i], &c[twomp1*threadID], &inc, &zero, &B[nnIndxLU[i]], &inc FCONE);
        F[i] = theta[sigmaSqIndx] - F77_NAME(ddot)(&nnIndxLU[n+i], &B[nnIndxLU[i]], &inc, &c[twomp1*threadID], &inc) + tauSq;                     // BJ
      } else {
        B[i] = 0;
        F[i] = theta[sigmaSqIndx] + tauSq;
      }
    }
  } else {
#ifdef _OPENMP
#pragma omp parallel for private(k, l, info, threadID, e)
#endif
    for(i = 0; i < n; i++){
#ifdef _OPENMP
      threadID = omp_get_thread_num();
#endif
      for(k = 0; k < nnIndxLUAll[n+i]; k++){
        if (k < m+1) {
          e = dist2(coords[i], coords[n+i],
                    coords[nnIndxParent[nnIndxLUParent[i]+k]],coords[n+nnIndxParent[nnIndxLUParent[i]+k]]);
          c[twomp1*threadID+k] = rho*theta[sigmaSqIndx]*
            spCor(e, theta[phiIndx], nu, covModel, &bk[threadID*nb]);
        } else {
          int k2 = k-m-1;
          e = dist2(coords[i], coords[n+i],
                    coords[nnIndx[nnIndxLU[i]+k2]], coords[n+nnIndx[nnIndxLU[i]+k2]]);
          c[twomp1*threadID+k] = theta[sigmaSqIndx]*
            spCor(e, theta[phiIndx], nu, covModel, &bk[threadID*nb]);
        }
        
        for(l = 0; l <= k; l++){
          if (k < m+1) {
            e = dist2(coords[nnIndxParent[nnIndxLUParent[i]+k]],coords[n+nnIndxParent[nnIndxLUParent[i]+k]],
                      coords[nnIndxParent[nnIndxLUParent[i]+l]],coords[n+nnIndxParent[nnIndxLUParent[i]+l]]);
            C[twomp1sq*threadID+l*nnIndxLUAll[n+i]+k] = theta[sigmaSqIndx]*
              spCor(e, theta[phiIndx], nu, covModel, &bk[threadID*nb]);
            if (l == k) {
              C[twomp1sq*threadID+l*nnIndxLUAll[n+i]+k] += tauSqParent;
            }
          } else {
            int k2 = k-m-1;
            if (l < m+1) {
              e = dist2(coords[nnIndx[nnIndxLU[i]+k2]],coords[n+nnIndx[nnIndxLU[i]+k2]],
                        coords[nnIndxParent[nnIndxLUParent[i]+l]],coords[n+nnIndxParent[nnIndxLUParent[i]+l]]);
              C[twomp1sq*threadID+l*nnIndxLUAll[n+i]+k] = rho*theta[sigmaSqIndx]*
                spCor(e, theta[phiIndx], nu, covModel, &bk[threadID*nb]);
            } else {
              int l2 = l-m-1;
              e = dist2(coords[nnIndx[nnIndxLU[i]+k2]],coords[n+nnIndx[nnIndxLU[i]+k2]],
                        coords[nnIndx[nnIndxLU[i]+l2]],coords[n+nnIndx[nnIndxLU[i]+l2]]);
              C[twomp1sq*threadID+l*nnIndxLUAll[n+i]+k] = theta[sigmaSqIndx]*
                spCor(e, theta[phiIndx], nu, covModel, &bk[threadID*nb]);
              if (l == k) {
                C[twomp1sq*threadID+l*nnIndxLUAll[n+i]+k] += tauSq;
              }
            }
          }
        }
      }
      F77_NAME(dpotrf)(&lower, &nnIndxLUAll[n+i], &C[twomp1sq*threadID], &nnIndxLUAll[n+i], &info FCONE);
      if(info != 0){error("c++ error: dpotrf failed\n");}
      F77_NAME(dpotri)(&lower, &nnIndxLUAll[n+i], &C[twomp1sq*threadID], &nnIndxLUAll[n+i], &info FCONE);
      F77_NAME(dsymv)(&lower, &nnIndxLUAll[n+i], &one, &C[twomp1sq*threadID], &nnIndxLUAll[n+i], &c[twomp1*threadID], &inc, &zero, &B[nnIndxLUAll[i]], &inc FCONE);
      F[i] = theta[sigmaSqIndx] - F77_NAME(ddot)(&nnIndxLUAll[n+i], &B[nnIndxLUAll[i]], &inc, &c[twomp1*threadID], &inc) + tauSq;                     // BJ
    }
  }
  
  for(i = 0; i < n; i++){
    logDet += log(F[i]);
  }
  
  return(logDet);
}

extern "C" {
  
  SEXP rSTNNGP(SEXP y_r, SEXP X_r, 
               SEXP q_r,                                                     // BJ
               SEXP p_r, SEXP n_r, SEXP m_r, SEXP coords_r, 
               SEXP covModel_r, SEXP nnIndx_r, SEXP nnIndxLU_r, 
               SEXP nnIndxParent_r, SEXP nnIndxLUParent_r, SEXP nnIndxLUAll_r, // BJ
               SEXP sigmaSqIG_r, 
               SEXP tauSqIGa_r, SEXP tauSqIGb_r,                             // BJ
               SEXP phiUnif_r, SEXP nuUnif_r, 
               SEXP rhoUnif_r,                                               // BJ
               SEXP betaStarting_r, SEXP sigmaSqStarting_r, SEXP tauSqStarting_r, 
               SEXP phiStarting_r, SEXP nuStarting_r, 
               SEXP rhoStarting_r, SEXP adjmatStarting_r,                    // BJ
               SEXP sigmaSqTuning_r, SEXP tauSqTuning_r, SEXP phiTuning_r, SEXP nuTuning_r, 
               SEXP rhoTuning_r,                                             // BJ
               SEXP nSamples_r, SEXP nThreads_r, SEXP verbose_r, SEXP nReport_r){
    
    int h, i, j, k, l, s, info, nProtect=0;
    const int inc = 1;
    const double one = 1.0;
    const double negOne = -1.0;
    const double zero = 0.0;
    char const *lower = "L";
    char const *ntran = "N";
    
    //get args
    double *y = REAL(y_r);
    double *X = REAL(X_r);
    int q = INTEGER(q_r)[0];                                                    // BJ
    int p = INTEGER(p_r)[0];
    int n = INTEGER(n_r)[0];
    int m = INTEGER(m_r)[0];
    double *coords = REAL(coords_r);
    int *nnIndx = INTEGER(nnIndx_r);
    int *nnIndxLU = INTEGER(nnIndxLU_r);
    int *nnIndxParent = INTEGER(nnIndxParent_r);                                // BJ
    int *nnIndxLUParent = INTEGER(nnIndxLUParent_r);                            // BJ
    int *nnIndxLUAll = INTEGER(nnIndxLUAll_r);                                  // BJ
    int covModel = INTEGER(covModel_r)[0];
    std::string corName = getCorName(covModel);
    
    //priors
    double sigmaSqIGa = REAL(sigmaSqIG_r)[0]; double sigmaSqIGb = REAL(sigmaSqIG_r)[1];
    double phiUnifa = REAL(phiUnif_r)[0]; double phiUnifb = REAL(phiUnif_r)[1];
    double nuUnifa = 0, nuUnifb = 0;
    if(corName == "matern"){
      nuUnifa = REAL(nuUnif_r)[0]; nuUnifb = REAL(nuUnif_r)[1]; 
    }
    double *tauSqIGa = REAL(tauSqIGa_r);                                        // BJ 
    double *tauSqIGb = REAL(tauSqIGb_r);                                        // BJ 
    double rhoUnifa = REAL(rhoUnif_r)[0]; double rhoUnifb = REAL(rhoUnif_r)[1]; // BJ
    
    int nSamples = INTEGER(nSamples_r)[0];
    int nThreads = INTEGER(nThreads_r)[0];
    int verbose = INTEGER(verbose_r)[0];
    int nReport = INTEGER(nReport_r)[0];
    
#ifdef _OPENMP
    omp_set_num_threads(nThreads);
#else
    if(nThreads > 1){
      warning("n.omp.threads > %i, but source not compiled with OpenMP support.", nThreads);
      nThreads = 1;
    }
#endif
    
    if(verbose){
      Rprintf("----------------------------------------\n");
      Rprintf("\tModel description\n");
      Rprintf("----------------------------------------\n");
      Rprintf("NNGP Response model fit with %i observations.\n\n", n);
      Rprintf("Number of covariates %i (including intercept if specified).\n\n", p);
      Rprintf("Using the %s spatial correlation model.\n\n", corName.c_str());
      Rprintf("Using %i nearest neighbors.\n\n", m);
      Rprintf("Number of MCMC samples %i.\n\n", nSamples);
      Rprintf("Priors and hyperpriors:\n");
      Rprintf("\tbeta flat.\n");
      Rprintf("\tsigma.sq IG hyperpriors shape=%.5f and scale=%.5f\n", sigmaSqIGa, sigmaSqIGb);
      for (int r = 0; r < q; r++) {                                             // BJ
        Rprintf("\ttau.sq[%i] IG hyperpriors shape=%.5f and scale=%.5f\n",      // BJ
                r+1, tauSqIGa[r], tauSqIGb[r]);                                 // BJ
      }                                                                         // BJ
      Rprintf("\tphi Unif hyperpriors a=%.5f and b=%.5f\n", phiUnifa, phiUnifb);
      if(corName == "matern"){
        Rprintf("\tnu Unif hyperpriors a=%.5f and b=%.5f\n", nuUnifa, nuUnifb);	  
      }
      Rprintf("\trho Unif hyperpriors a=%.5f and b=%.5f\n", rhoUnifa, rhoUnifb);// BJ
#ifdef _OPENMP
      Rprintf("\nSource compiled with OpenMP support and model fit using %i thread(s).\n", nThreads);
#else
      Rprintf("\n\nSource not compiled with OpenMP support.\n");
#endif
    } 
    
    //parameters                                                                // BJ: remove tauSqIndx
    int nTheta, sigmaSqIndx, phiIndx, nuIndx;                                   // BJ
    
    if(corName != "matern"){                                                    // BJ
      nTheta = 2;//sigma^2, phi                                                 // BJ
      sigmaSqIndx = 0; phiIndx = 1;                                             // BJ
    }else{                                                                      // BJ
      nTheta = 3;//sigma^2, phi, nu                                             // BJ
      sigmaSqIndx = 0; phiIndx = 1; nuIndx = 2;                                 // BJ 
    }                                                                           // BJ
    
    //starting	
    int pq = p*q;                                                               // BJ
    int qm1 = q-1;                                                              // BJ
    double *beta = (double *) R_alloc(pq, sizeof(double));                      // BJ
    double *betaj = (double *) R_alloc(p, sizeof(double));                      // BJ
    double *beta2 = (double *) R_alloc(pq, sizeof(double));                     // BJ
    double *beta2j = (double *) R_alloc(p, sizeof(double));                     // BJ
    double *theta = (double *) R_alloc(nTheta, sizeof(double));
    double *tauSq = (double *) R_alloc(q, sizeof(double));                      // BJ
    double *rho = (double *) R_alloc(qm1, sizeof(double));                      // BJ
    int *adjvec = INTEGER(adjmatStarting_r);                                    // BJ
    
    F77_NAME(dcopy)(&pq, REAL(betaStarting_r), &inc, beta, &inc);               // BJ
    F77_NAME(dcopy)(&pq, REAL(betaStarting_r), &inc, beta2, &inc);              // BJ
    theta[sigmaSqIndx] = REAL(sigmaSqStarting_r)[0];
    theta[phiIndx] = REAL(phiStarting_r)[0];
    if(corName == "matern"){
      theta[nuIndx] = REAL(nuStarting_r)[0];
    }
    F77_NAME(dcopy)(&q, REAL(tauSqStarting_r), &inc, tauSq, &inc);              // BJ
    F77_NAME(dcopy)(&qm1, REAL(rhoStarting_r), &inc, rho, &inc);                // BJ

    //tuning and fixed
    double *tuning = (double *) R_alloc(nTheta, sizeof(double));
    
    tuning[sigmaSqIndx] = REAL(sigmaSqTuning_r)[0];
    tuning[phiIndx] = REAL(phiTuning_r)[0];
    if(corName == "matern"){
      tuning[nuIndx] = REAL(nuTuning_r)[0];
    }
    double *tauSqTuning = REAL(tauSqTuning_r);                                  // BJ
    double *rhoTuning = REAL(rhoTuning_r);                                      // BJ
    
    //return stuff  
    SEXP betaSamples_r, thetaSamples_r, tauSqSamples_r, rhoSamples_r;           // BJ
    PROTECT(betaSamples_r = allocMatrix(REALSXP, pq, nSamples)); nProtect++;    // BJ
    PROTECT(thetaSamples_r = allocMatrix(REALSXP, nTheta, nSamples)); nProtect++; 
    PROTECT(tauSqSamples_r = allocMatrix(REALSXP, q, nSamples)); nProtect++;    // BJ
    PROTECT(rhoSamples_r = allocMatrix(REALSXP, qm1, nSamples)); nProtect++;    // BJ

    //miscellaneous
    int nIndx = static_cast<int>(static_cast<double>(m*(m-1)/2+(n-m)*m+(m+1)*n)); // BJ
    int twomp1 = 2*m+1;                                                         // BJ
    int twomp1sq = pow(twomp1,2);                                               // BJ
    double *thetaCand = (double *) R_alloc(nTheta, sizeof(double));
    double *tauSqCand = (double *) R_alloc(q, sizeof(double));                  // BJ
    F77_NAME(dcopy)(&q, tauSq, &inc, tauSqCand, &inc);                          // BJ
    double *rhoCand = (double *) R_alloc(qm1, sizeof(double));                  // BJ
    double *B = (double *) R_alloc(nIndx, sizeof(double));                      // BJ
    double *F = (double *) R_alloc(n, sizeof(double));                          // BJ
    double *c =(double *) R_alloc(twomp1*nThreads, sizeof(double));             // BJ
    double *C = (double *) R_alloc(twomp1sq*nThreads, sizeof(double));          // BJ
    
    double logPostCand, logPostCurrent, logDetCurrent, logDetCand, QCurrent, QCand;     
    double QCurrent2;                                                           // BJ
    int accept = 0, batchAccept = 0, status = 0;
    int pp = p*p;
    double *tmp_pp = (double *) R_alloc(pp, sizeof(double));
    double *tmp_p = (double *) R_alloc(p, sizeof(double));
    double *tmp_p2 = (double *) R_alloc(p, sizeof(double));
    double *tmp_n = (double *) R_alloc(n, sizeof(double));
    double *tmp_n_parent = (double *) R_alloc(n, sizeof(double));               // BJ
    double *tmp_n2 = NULL;
    double *tmp_m = NULL;
    double *bk = (double *) R_alloc(nThreads*(1.0+static_cast<int>(floor(nuUnifb))), sizeof(double));
    double *tmp_zero = (double *) R_alloc(n, sizeof(double));                   // BJ
    zeros(tmp_zero, n);                                                         // BJ

    bool thetaUpdate = true;
    bool root; 
    int ParentIndx;
    
    if(verbose){
      Rprintf("----------------------------------------\n");
      Rprintf("\t\tSampling\n");
      Rprintf("----------------------------------------\n");
#ifdef Win32
      R_FlushConsole();
#endif
    }
    
    GetRNGstate();
    
    for(s = 0; s < nSamples; s++){
      if(thetaUpdate){                                                          // BJ: need to save c, C, B, F, tmp_ to activate this and save time
        thetaUpdate = false;
        
        root = true;                                                            // BJ
        ParentIndx = 0;                                                         // BJ: for MeIndx = 0, does not affect results
        logDetCurrent = 0; 
        QCurrent = 0;
        QCurrent2 = 0;                                                          // BJ
        
        for (int MeIndx = 0; MeIndx < q; MeIndx++) {                            // BJ
          if (MeIndx > 0) {                                                     // BJ
            root = false;                                                       // BJ
            ParentIndx = which(1, &adjvec[q*MeIndx], q);                        // BJ
          }                                                                     // BJ
          
          ///////////////////
          // BJ: update beta
          //////////////////
          // BJ: need B and F to update beta
          logDetCurrent += updateBF_parent_r(B, F, c, C, coords,
                                             nnIndx, nnIndxLU,
                                             nnIndxParent, nnIndxLUParent, nnIndxLUAll,
                                             n, m, twomp1, twomp1sq,
                                             theta, sigmaSqIndx, phiIndx, nuIndx,
                                             tauSq[MeIndx], tauSq[ParentIndx], rho[MeIndx-1], 
                                             covModel, bk, nuUnifb, root);
          
          for(i = 0; i < p; i++){
            tmp_p[i] = Q_parent(B, F, &X[n*i], &y[n*MeIndx], tmp_zero, tmp_zero,
                                n, nnIndx, nnIndxLU, nnIndxParent, nnIndxLUParent,
                                nnIndxLUAll, root);
            for(j = 0; j <= i; j++){
              tmp_pp[j*p+i] = Q_parent(B, F, &X[n*j], &X[n*i], tmp_zero, tmp_zero,
                                       n, nnIndx, nnIndxLU, nnIndxParent, nnIndxLUParent,
                                       nnIndxLUAll, root);
            }
          }
          
          F77_NAME(dpotrf)(lower, &p, tmp_pp, &p, &info FCONE);
          if(info != 0){
            error("c++ error: dpotrf failed\n");
          }
          F77_NAME(dpotri)(lower, &p, tmp_pp, &p, &info FCONE);
          if(info != 0){
            error("c++ error: dpotri failed\n");
          }
          F77_NAME(dsymv)(lower, &p, &one, tmp_pp, &p, tmp_p, &inc, &zero, tmp_p2, &inc FCONE);
          F77_NAME(dpotrf)(lower, &p, tmp_pp, &p, &info FCONE);
          if(info != 0){
            error("c++ error: dpotrf failed\n");
          }
          // std::cout << "beta posterior mean " << tmp_p2[0] << ", " << tmp_p2[1] << "\n";
          // std::cout << "beta posterior var " << tmp_pp[0] << ", " << tmp_pp[1] << ", " << tmp_pp[2] << ", " << tmp_pp[3] << "\n";
          
          mvrnorm(betaj, tmp_p2, tmp_pp, p);
          F77_NAME(dcopy)(&p, betaj, &inc, &beta[p*MeIndx], &inc);
          // std::cout << "beta " << betaj[0] << ", " << betaj[1] << "\n";
          
          F77_NAME(dgemv)(ntran, &n, &p, &one, X, &n, &beta[p*MeIndx], &inc, &zero, tmp_n, &inc FCONE);
          F77_NAME(daxpy)(&n, &negOne, &y[n*MeIndx], &inc, tmp_n, &inc);
          F77_NAME(dgemv)(ntran, &n, &p, &one, X, &n, &beta[p*ParentIndx], &inc, &zero, tmp_n_parent, &inc FCONE); 
          F77_NAME(daxpy)(&n, &negOne, &y[n*ParentIndx], &inc, tmp_n_parent, &inc);            
          QCurrent += Q_parent(B, F, tmp_n, tmp_n, tmp_n_parent, tmp_n_parent,
                               n, nnIndx, nnIndxLU,
                               nnIndxParent, nnIndxLUParent, nnIndxLUAll, root);  
          
          // BJ: backup in case theta is not updated
          mvrnorm(beta2j, tmp_p2, tmp_pp, p);
          F77_NAME(dcopy)(&p, beta2j, &inc, &beta2[p*MeIndx], &inc);
          
          F77_NAME(dgemv)(ntran, &n, &p, &one, X, &n, &beta2[p*MeIndx], &inc, &zero, tmp_n, &inc FCONE);
          F77_NAME(daxpy)(&n, &negOne, &y[n*MeIndx], &inc, tmp_n, &inc);
          F77_NAME(dgemv)(ntran, &n, &p, &one, X, &n, &beta2[p*ParentIndx], &inc, &zero, tmp_n_parent, &inc FCONE); 
          F77_NAME(daxpy)(&n, &negOne, &y[n*ParentIndx], &inc, tmp_n_parent, &inc);            
          QCurrent2 += Q_parent(B, F, tmp_n, tmp_n, tmp_n_parent, tmp_n_parent,
                                n, nnIndx, nnIndxLU,
                                nnIndxParent, nnIndxLUParent, nnIndxLUAll, root);  
        }
      } else {
        QCurrent = QCurrent2;                                                   // BJ
        F77_NAME(dcopy)(&pq, beta2, &inc, beta, &inc);                          // BJ
      }
      // Rprintf("\tlogDet=%f\n", logDetCurrent);                                    // BJ
      // Rprintf("\tQ=%f\n", QCurrent);                                              // BJ
      // std::cout << "beta: (" << beta[0] << ", " << beta[1] << ", " << beta[2] << ", " << beta[3] << ", " << beta[4] << ", " << beta[5] << ")" << "\n";
      
      /////////////////////
      // BJ: update theta
      ////////////////////
      logPostCurrent = -0.5*logDetCurrent - 0.5*QCurrent;
      logPostCurrent += log(theta[phiIndx] - phiUnifa) + log(phiUnifb - theta[phiIndx]);
      logPostCurrent += -1.0*(1.0+sigmaSqIGa)*log(theta[sigmaSqIndx])-sigmaSqIGb/theta[sigmaSqIndx]+log(theta[sigmaSqIndx]);
      if(corName == "matern"){
        logPostCurrent += log(theta[nuIndx] - nuUnifa) + log(nuUnifb - theta[nuIndx]);
      }
      // BJ
      for (int o = 1; o < q; o++) {
        logPostCurrent += log(rho[o-1] - rhoUnifa) + log(rhoUnifb - rho[o-1]);
      }
      for (int o = 0; o < q; o++) {
        if (tauSq[o] != 0) {
          logPostCurrent += -1.0*(1.0+tauSqIGa[o])*log(tauSq[o])-tauSqIGb[o]/tauSq[o]+log(tauSq[o]);
        }
      }
      // Rprintf("\tlogPostCurrent=%f\n", logPostCurrent);                         // BJ
      
      //candidate
      thetaCand[phiIndx] = logitInv(rnorm(logit(theta[phiIndx], phiUnifa, phiUnifb), tuning[phiIndx]), phiUnifa, phiUnifb);
      // std::cout << "phi: " << thetaCand[phiIndx] << "\n";
      thetaCand[sigmaSqIndx] = exp(rnorm(log(theta[sigmaSqIndx]), tuning[sigmaSqIndx]));
      // std::cout << "sigma.sq: " << thetaCand[sigmaSqIndx] << "\n";
      if(corName == "matern"){
        thetaCand[nuIndx] = logitInv(rnorm(logit(theta[nuIndx], nuUnifa, nuUnifb), tuning[nuIndx]), nuUnifa, nuUnifb);
      }
      // BJ
      for (int o = 1; o < q; o++) {
        rhoCand[o-1] = logitInv(rnorm(logit(rho[o-1], rhoUnifa, rhoUnifb), rhoTuning[o-1]), rhoUnifa, rhoUnifb);
      }
      // std::cout << "rho: (" << rhoCand[0] << ", " << rhoCand[1] << ", " << rhoCand[2] << ")" << "\n";
      for (int o = 0; o < q; o++) {
        if (tauSqTuning[o] != 0) {
          tauSqCand[o] = exp(rnorm(log(tauSq[o]), tauSqTuning[o]));
        }
      }
      
      //update logDet and Q
      root = true;                                                            // BJ
      ParentIndx = 0;                                                         // BJ: for MeIndx = 0, does not affect results
      logDetCand = 0;
      QCand = 0;
      
      for (int MeIndx = 0; MeIndx < q; MeIndx++) {                                // BJ
        if (MeIndx > 0) {                                                         // BJ
          root = false;                                                           // BJ
          ParentIndx = which(1, &adjvec[q*MeIndx], q);                            // BJ
        }                                                                         // BJ
        
        logDetCand += updateBF_parent_r(B, F, c, C, coords,
                                        nnIndx, nnIndxLU,
                                        nnIndxParent, nnIndxLUParent, nnIndxLUAll,
                                        n, m, twomp1, twomp1sq,
                                        thetaCand, sigmaSqIndx, phiIndx, nuIndx,
                                        tauSqCand[MeIndx], tauSqCand[ParentIndx], 
                                        rhoCand[MeIndx-1], 
                                        covModel, bk, nuUnifb, root);
        
        F77_NAME(dgemv)(ntran, &n, &p, &one, X, &n, &beta[p*MeIndx], &inc, &zero, tmp_n, &inc FCONE);
        F77_NAME(daxpy)(&n, &negOne, &y[n*MeIndx], &inc, tmp_n, &inc);
        F77_NAME(dgemv)(ntran, &n, &p, &one, X, &n, &beta[p*ParentIndx], &inc, &zero, tmp_n_parent, &inc FCONE); 
        F77_NAME(daxpy)(&n, &negOne, &y[n*ParentIndx], &inc, tmp_n_parent, &inc);            
        QCand += Q_parent(B, F, tmp_n, tmp_n, tmp_n_parent, tmp_n_parent,
                          n, nnIndx, nnIndxLU,
                          nnIndxParent, nnIndxLUParent, nnIndxLUAll, root);            
      }
      
      logPostCand = -0.5*logDetCand - 0.5*QCand;
      logPostCand += log(thetaCand[phiIndx] - phiUnifa) + log(phiUnifb - thetaCand[phiIndx]);
      logPostCand += -1.0*(1.0+sigmaSqIGa)*log(thetaCand[sigmaSqIndx])-sigmaSqIGb/thetaCand[sigmaSqIndx]+log(thetaCand[sigmaSqIndx]);
      if(corName == "matern"){
        logPostCand += log(thetaCand[nuIndx] - nuUnifa) + log(nuUnifb - thetaCand[nuIndx]);
      }
      for (int o = 1; o < q; o++) {
        logPostCand += log(rhoCand[o-1] - rhoUnifa) + log(rhoUnifb - rhoCand[o-1]);
      }
      for (int o = 0; o < q; o++) {
        if (tauSqCand[o] != 0) {
          logPostCand += -1.0*(1.0+tauSqIGa[o])*log(tauSqCand[o])-tauSqIGb[o]/tauSqCand[o]+log(tauSq[o]);
        }
      }
      // Rprintf("\tlogDetCand=%f\n", logDetCand);
      // Rprintf("\tQCand=%f\n", QCand);
      // Rprintf("\tlogPostCand=%f\n", logPostCand);
      
      if (runif(0.0, 1.0) <= exp(logPostCand - logPostCurrent)) {
        thetaUpdate = true;
        dcopy_(&nTheta, thetaCand, &inc, theta, &inc);
        dcopy_(&qm1, rhoCand, &inc, rho, &inc);                                 // BJ
        dcopy_(&q, tauSqCand, &inc, tauSq, &inc);
        accept++;
        batchAccept++;
      }
      //save samples
      F77_NAME(dcopy)(&pq, beta, &inc, &REAL(betaSamples_r)[s*pq], &inc);
      F77_NAME(dcopy)(&nTheta, theta, &inc, &REAL(thetaSamples_r)[s*nTheta], &inc);
      F77_NAME(dcopy)(&qm1, rho, &inc, &REAL(rhoSamples_r)[s*qm1], &inc);
      F77_NAME(dcopy)(&q, tauSq, &inc, &REAL(tauSqSamples_r)[s*q], &inc);
      
      //report
      if(status == nReport){
        if(verbose){
          Rprintf("Sampled: %i of %i, %3.2f%%\n", s, nSamples, 100.0*s/nSamples);
          Rprintf("Report interval Metrop. Acceptance rate: %3.2f%%\n", 100.0*batchAccept/nReport);
          Rprintf("Overall Metrop. Acceptance rate: %3.2f%%\n", 100.0*accept/s);
          Rprintf("-------------------------------------------------\n");
#ifdef Win32
          R_FlushConsole();
#endif
        }
        batchAccept = 0;
        status = 0;
      }
      
      status++;
      
      R_CheckUserInterrupt();
    }
    
    if(verbose){
      Rprintf("Sampled: %i of %i, %3.2f%%\n", s, nSamples, 100.0);
      Rprintf("Report interval Metrop. Acceptance rate: %3.2f%%\n", 100.0*batchAccept/nReport);
      Rprintf("Overall Metrop. Acceptance rate: %3.2f%%\n", 100.0*accept/nSamples);
      Rprintf("-------------------------------------------------\n");
#ifdef Win32
      R_FlushConsole();
#endif
    }
    
    PutRNGstate();
    
    //make return object
    SEXP result_r, resultName_r;
    int nResultListObjs = 4;
    
    PROTECT(result_r = allocVector(VECSXP, nResultListObjs)); nProtect++;
    PROTECT(resultName_r = allocVector(VECSXP, nResultListObjs)); nProtect++;
    
    SET_VECTOR_ELT(result_r, 0, betaSamples_r);
    SET_VECTOR_ELT(resultName_r, 0, mkChar("p.beta.samples"));
    
    SET_VECTOR_ELT(result_r, 1, thetaSamples_r);
    SET_VECTOR_ELT(resultName_r, 1, mkChar("p.theta.samples"));
    
    SET_VECTOR_ELT(result_r, 2, tauSqSamples_r);
    SET_VECTOR_ELT(resultName_r, 2, mkChar("p.tausq.samples"));
    
    SET_VECTOR_ELT(result_r, 3, rhoSamples_r);
    SET_VECTOR_ELT(resultName_r, 3, mkChar("p.rho.samples"));
    
    namesgets(result_r, resultName_r);
    
    //unprotect
    UNPROTECT(nProtect);
    
    return(result_r);
  }
}