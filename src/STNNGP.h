#include <R.h>
#include <Rinternals.h>
  
void updateBF_parent_NS(double *B, double *F, double *c, double *C, double *coords,
                          int *nnIndx, int *nnIndxLU,
                          int *nnIndxParent, int *nnIndxLUParent, int *nnIndxLUAll,
                          int n, int m, int nIndx, int twomp1, int twomp1sq, int q,
                          double *sigmaSq, double *phi, double *nu,
                          double *rho, double *crossphi, int *adjvec, 
                          int covModel, double nThreads, double *nuUnifb);

void updateBF_parent(double *B, double *F, double *c, double *C, double *coords,
                     int *nnIndx, int *nnIndxLU,
                     int *nnIndxParent, int *nnIndxLUParent, int *nnIndxLUAll,
                     int n, int m, int nIndx, int twomp1, int twomp1sq, int q,
                     double sigmaSq, double phi, double nu,
                     double *rho, 
                     int covModel, double *bk, double nuUnifb);

extern "C" {
  SEXP rSTNNGP(SEXP y_r, SEXP X_r, 
               SEXP q_r,                                                     
               SEXP p_r, SEXP n_r, SEXP m_r, SEXP coords_r, 
               SEXP covModel_r, SEXP nnIndx_r, SEXP nnIndxLU_r, 
               SEXP nnIndxParent_r, SEXP nnIndxLUParent_r, SEXP nnIndxLUAll_r, 
               SEXP sigmaSqIG_r, 
               SEXP tauSqIGa_r, SEXP tauSqIGb_r,                             
               SEXP phiUnif_r, SEXP nuUnif_r, 
               SEXP rhoUnif_r,                                               
               SEXP betaStarting_r, SEXP sigmaSqStarting_r, SEXP tauSqStarting_r, 
               SEXP phiStarting_r, SEXP nuStarting_r, 
               SEXP rhoStarting_r, SEXP adjmatStarting_r,                    
               SEXP sigmaSqTuning_r, SEXP tauSqTuning_r, SEXP phiTuning_r, SEXP nuTuning_r, 
               SEXP rhoTuning_r,                                             
               SEXP nSamples_r, SEXP nThreads_r, SEXP verbose_r, SEXP nReport_r);
  
  SEXP rSTNNGP_NS(SEXP y_r, SEXP X_r, 
                  SEXP q_r,                                                     
                  SEXP p_r, SEXP n_r, SEXP m_r, SEXP coords_r, 
                  SEXP covModel_r, SEXP nnIndx_r, SEXP nnIndxLU_r, 
                  SEXP nnIndxParent_r, SEXP nnIndxLUParent_r, SEXP nnIndxLUAll_r, 
                  SEXP sigmaSqIGa_r, SEXP sigmaSqIGb_r,                          
                  SEXP tauSqIGa_r, SEXP tauSqIGb_r,                             
                  SEXP phiUnifa_r, SEXP phiUnifb_r,                             
                  SEXP nuUnifa_r, SEXP nuUnifb_r,                               
                  SEXP betaStarting_r, SEXP sigmaSqStarting_r, SEXP tauSqStarting_r, 
                  SEXP phiStarting_r, 
                  SEXP crossphiStarting_r,                                      
                  SEXP nuStarting_r, 
                  SEXP rhoStarting_r, SEXP adjmatStarting_r,                    
                  SEXP sigmaSqTuning_r, SEXP tauSqTuning_r, SEXP phiTuning_r, 
                  SEXP crossphiTuning_r,                                        
                  SEXP nuTuning_r, 
                  SEXP rhoTuning_r,                                             
                  SEXP nSamples_r, SEXP nThreads_r, SEXP verbose_r, SEXP nReport_r);
  
  SEXP sSTNNGP(SEXP y_r, SEXP X_r, 
               SEXP q_r,                                                        
               SEXP p_r, SEXP n_r, SEXP m_r, SEXP coords_r, 
               SEXP covModel_r, SEXP nnIndx_r, SEXP nnIndxLU_r, 
               SEXP nnIndxParent_r, SEXP nnIndxLUParent_r, SEXP nnIndxLUAll_r,  
               SEXP uIndx_r, SEXP uIndxLU_r, SEXP uiIndx_r, 
               SEXP uuIndx_r, SEXP uuIndxLU_r, SEXP uuiIndx_r, 
               SEXP cIndx_r, SEXP cIndxLU_r,                                    
               SEXP sigmaSqIG_r, 
               SEXP tauSqIGa_r, SEXP tauSqIGb_r,                                
               SEXP phiUnif_r, SEXP nuUnif_r, 
               SEXP rhoUnif_r,                                                  
               SEXP betaStarting_r, SEXP sigmaSqStarting_r, SEXP tauSqStarting_r, 
               SEXP phiStarting_r, SEXP nuStarting_r, 
               SEXP rhoStarting_r, SEXP adjmatStarting_r,                       
               SEXP phiTuning_r, SEXP nuTuning_r, 
               SEXP rhoTuning_r,                                                
               SEXP nSamples_r, SEXP nThreads_r, SEXP verbose_r, SEXP nReport_r);
  
  SEXP sSTNNGP_NS(SEXP y_r, SEXP X_r, 
                  SEXP q_r,                                                     
                  SEXP p_r, SEXP n_r, SEXP m_r, SEXP coords_r, 
                  SEXP covModel_r, SEXP nnIndx_r, SEXP nnIndxLU_r, 
                  SEXP nnIndxParent_r, SEXP nnIndxLUParent_r, SEXP nnIndxLUAll_r, 
                  SEXP uIndx_r, SEXP uIndxLU_r, SEXP uiIndx_r,     
                  SEXP uuIndx_r, SEXP uuIndxLU_r, SEXP uuiIndx_r, 
                  SEXP cIndx_r, SEXP cIndxLU_r,                                 
                  SEXP sigmaSqIGa_r, SEXP sigmaSqIGb_r,                         
                  SEXP tauSqIGa_r, SEXP tauSqIGb_r,                             
                  SEXP phiUnifa_r, SEXP phiUnifb_r,                             
                  SEXP nuUnifa_r, SEXP nuUnifb_r,                               
                  SEXP betaStarting_r, SEXP sigmaSqStarting_r, SEXP tauSqStarting_r, 
                  SEXP phiStarting_r, 
                  SEXP crossphiStarting_r,                                      
                  SEXP nuStarting_r, 
                  SEXP rhoStarting_r, SEXP adjmatStarting_r,                    
                  SEXP sigmaSqTuning_r,                                         
                  SEXP phiTuning_r, 
                  SEXP crossphiTuning_r,                                        
                  SEXP nuTuning_r, 
                  SEXP rhoTuning_r,                                             
                  SEXP nSamples_r, SEXP nThreads_r, SEXP verbose_r, SEXP nReport_r);
  
  SEXP sSTNNGP_misalign(SEXP y_r, SEXP X_r, 
                        SEXP XtX_r,                                             
                        SEXP q_r,                                               
                        SEXP p_r, SEXP n_r, SEXP m_r, SEXP coords_r, 
                        SEXP nj_r, SEXP Y_missing_r,                            
                        SEXP covModel_r, SEXP nnIndx_r, SEXP nnIndxLU_r, 
                        SEXP nnIndxParent_r, SEXP nnIndxLUParent_r, SEXP nnIndxLUAll_r,  
                        SEXP uIndx_r, SEXP uIndxLU_r, SEXP uiIndx_r,    
                        SEXP uuIndx_r, SEXP uuIndxLU_r, SEXP uuiIndx_r, 
                        SEXP cIndx_r, SEXP cIndxLU_r,                           
                        SEXP sigmaSqIG_r, 
                        SEXP tauSqIGa_r, SEXP tauSqIGb_r,                       
                        SEXP phiUnif_r, SEXP nuUnif_r, 
                        SEXP rhoUnif_r,                                         
                        SEXP betaStarting_r, SEXP sigmaSqStarting_r, SEXP tauSqStarting_r, 
                        SEXP phiStarting_r, SEXP nuStarting_r, 
                        SEXP rhoStarting_r, SEXP adjmatStarting_r,        
                        SEXP wStarting_r,
                        SEXP phiTuning_r, SEXP nuTuning_r, 
                        SEXP rhoTuning_r,                                       
                        SEXP nSamples_r, SEXP nThreads_r, SEXP verbose_r, SEXP nReport_r);
  
  SEXP sSTNNGP_NS_misalign(SEXP y_r, SEXP X_r, 
                           SEXP XtX_r,                                          
                           SEXP q_r,                                            
                           SEXP p_r, SEXP n_r, SEXP m_r, SEXP coords_r, 
                           SEXP nj_r, SEXP Y_missing_r,                         
                           SEXP covModel_r, SEXP nnIndx_r, SEXP nnIndxLU_r, 
                           SEXP nnIndxParent_r, SEXP nnIndxLUParent_r, SEXP nnIndxLUAll_r, 
                           SEXP uIndx_r, SEXP uIndxLU_r, SEXP uiIndx_r,     
                           SEXP uuIndx_r, SEXP uuIndxLU_r, SEXP uuiIndx_r, 
                           SEXP cIndx_r, SEXP cIndxLU_r,                                 
                           SEXP sigmaSqIGa_r, SEXP sigmaSqIGb_r,                         
                           SEXP tauSqIGa_r, SEXP tauSqIGb_r,                             
                           SEXP phiUnifa_r, SEXP phiUnifb_r,                             
                           SEXP nuUnifa_r, SEXP nuUnifb_r,                               
                           SEXP betaStarting_r, SEXP sigmaSqStarting_r, SEXP tauSqStarting_r, 
                           SEXP phiStarting_r, 
                           SEXP crossphiStarting_r,                                      
                           SEXP nuStarting_r, 
                           SEXP rhoStarting_r, SEXP adjmatStarting_r,                    
                           SEXP sigmaSqTuning_r,                                         
                           SEXP phiTuning_r, 
                           SEXP crossphiTuning_r,                                        
                           SEXP nuTuning_r, 
                           SEXP rhoTuning_r,                                             
                           SEXP nSamples_r, SEXP nThreads_r, SEXP verbose_r, SEXP nReport_r);
    
    SEXP sSTNNGPPredict(SEXP X_r, SEXP y_r, SEXP coords_r, SEXP adjvec_r, 
                        SEXP n_r, SEXP q_r, SEXP p_r, SEXP m_r, 
                        SEXP X0_r, SEXP coords0_r, SEXP n0_r, SEXP nnIndx0_r, 
                        SEXP betaSamples_r, SEXP tauSqSamples_r, SEXP rhoSamples_r, 
                        SEXP thetaSamples_r, SEXP wSamples_r, 
                        SEXP nSamples_r, SEXP family_r, SEXP covModel_r, SEXP nThreads_r, 
                        SEXP verbose_r, SEXP nReport_r);
    
    SEXP sSTNNGPPredict_NS(SEXP X_r, SEXP y_r, SEXP coords_r, SEXP adjvec_r, 
                           SEXP n_r, SEXP q_r, SEXP p_r, SEXP m_r, 
                           SEXP X0_r, SEXP coords0_r, SEXP n0_r, SEXP nnIndx0_r, 
                           SEXP betaSamples_r, SEXP tauSqSamples_r, SEXP rhoSamples_r, 
                           SEXP sigmaSqSamples_r, SEXP phiSamples_r, 
                           SEXP crossphiSamples_r, SEXP nuSamples_r, SEXP wSamples_r, 
                           SEXP nSamples_r, SEXP family_r, SEXP covModel_r, SEXP nThreads_r, 
                           SEXP verbose_r, SEXP nReport_r);
    
    SEXP sSTNNGPLogit(SEXP y_r, SEXP X_r, 
                      SEXP q_r,                                                   
                      SEXP p_r, SEXP n_r, SEXP m_r, SEXP coords_r, 
                      SEXP Y_missing_r,                                           
                      SEXP nTrial_r,                                              
                      SEXP covModel_r, SEXP nnIndx_r, SEXP nnIndxLU_r, 
                      SEXP nnIndxParent_r, SEXP nnIndxLUParent_r, SEXP nnIndxLUAll_r,  
                      SEXP uIndx_r, SEXP uIndxLU_r, SEXP uiIndx_r,     
                      SEXP uuIndx_r, SEXP uuIndxLU_r, SEXP uuiIndx_r,             
                      SEXP cIndx_r, SEXP cIndxLU_r,                               
                      SEXP sigmaSqIG_r, 
                      SEXP phiUnif_r, SEXP nuUnif_r, 
                      SEXP rhoUnif_r,                                             
                      SEXP betaStarting_r, SEXP sigmaSqStarting_r, 
                      SEXP phiStarting_r, SEXP nuStarting_r, 
                      SEXP rhoStarting_r, SEXP adjmatStarting_r,    
                      SEXP wStarting_r, 
                      SEXP phiTuning_r, SEXP nuTuning_r, 
                      SEXP rhoTuning_r,                                           
                      SEXP nSamples_r, SEXP nThreads_r, SEXP verbose_r, SEXP nReport_r, 
                      SEXP updatesigmaSq_r, SEXP updatebeta_r, SEXP updatew_r);
    
    SEXP sSTNNGPLogit_NS(SEXP y_r, SEXP X_r, 
                         SEXP q_r,                                                
                         SEXP p_r, SEXP n_r, SEXP m_r, SEXP coords_r, 
                         SEXP Y_missing_r,                                        
                         SEXP nTrial_r,                                           
                         SEXP covModel_r, SEXP nnIndx_r, SEXP nnIndxLU_r, 
                         SEXP nnIndxParent_r, SEXP nnIndxLUParent_r, SEXP nnIndxLUAll_r, 
                         SEXP uIndx_r, SEXP uIndxLU_r, SEXP uiIndx_r,     
                         SEXP uuIndx_r, SEXP uuIndxLU_r, SEXP uuiIndx_r,          
                         SEXP cIndx_r, SEXP cIndxLU_r,                            
                         SEXP sigmaSqIGa_r, SEXP sigmaSqIGb_r,                    
                         SEXP phiUnifa_r, SEXP phiUnifb_r,                        
                         SEXP nuUnifa_r, SEXP nuUnifb_r,                          
                         SEXP betaStarting_r, SEXP sigmaSqStarting_r, 
                         SEXP phiStarting_r, 
                         SEXP crossphiStarting_r,                                 
                         SEXP nuStarting_r, 
                         SEXP rhoStarting_r, SEXP adjmatStarting_r,  
                         SEXP wStarting_r, 
                         SEXP sigmaSqTuning_r,                                    
                         SEXP phiTuning_r, 
                         SEXP crossphiTuning_r,                                   
                         SEXP nuTuning_r, 
                         SEXP rhoTuning_r,                                        
                         SEXP nSamples_r, SEXP nThreads_r, SEXP verbose_r, SEXP nReport_r);
}
