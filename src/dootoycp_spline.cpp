#include "string"
#include "vector"
#include "iostream"
#include "RooBMixDecay.h"
#include "RooCategory.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooGenericPdf.h"
#include "RooPlot.h"
#include "RooRealVar.h"
#include "RooSimPdfBuilder.h"
#include "RooSimultaneous.h"
#include "RooWorkspace.h"
#include "RooSimWSTool.h"

#include "doofit/builder/EasyPdf/EasyPdf.h"
#include "doofit/config/CommonConfig.h"
#include "doofit/plotting/fitresult/FitResultPrinter.h"
#include "doofit/fitter/easyfit/EasyFit.h"
#include "doofit/toy/ToyFactoryStd/ToyFactoryStd.h"
#include "doofit/toy/ToyFactoryStd/ToyFactoryStdConfig.h"
#include "doofit/toy/ToyStudyStd/ToyStudyStd.h"
#include "doofit/toy/ToyStudyStd/ToyStudyStdConfig.h"

#include "doofit/plotting/Plot/Plot.h"
#include "doofit/plotting/Plot/PlotConfig.h"

#include "RooAbsPdf.h"
#include "RooExtendPdf.h"
#include "Urania/DecRateCoeff.h"
#include "doofit/roofit/pdfs/DooCubicSplinePdf.h"


int main(int argc, char* argv[]) {

     doofit::builder::EasyPdf *epdf = new doofit::builder::EasyPdf();

     

    epdf->Var("sig_yield");
    epdf->Var("sig_yield").setVal(153000);
    epdf->Var("sig_yield").setConstant(false);
    //decay time
    epdf->Var("obsTime");
    epdf->Var("obsTime").SetTitle("t_{#kern[-0.2]{B}_{#kern[-0.1]{ d}}^{#kern[-0.1]{ 0}}}");
    epdf->Var("obsTime").setUnit("ps");
    epdf->Var("obsTime").setRange(0.,16.);

    // tag, respectively the initial state of the produced B meson
    epdf->Cat("obsTag");
    epdf->Cat("obsTag").defineType("B_S",1);
    epdf->Cat("obsTag").defineType("Bbar_S",-1);

    //finalstate
    epdf->Cat("catFinalState");
    epdf->Cat("catFinalState").defineType("f",1);
    epdf->Cat("catFinalState").defineType("fbar",-1);

    epdf->Var("obsEtaOS");
    epdf->Var("obsEtaOS").setRange(0.0,0.5);


     std::vector<double> knots;
            knots.push_back(0.07);
            knots.push_back(0.10);
            knots.push_back(0.138);
            knots.push_back(0.16);
            knots.push_back(0.23);
            knots.push_back(0.28);
            knots.push_back(0.35);
            knots.push_back(0.42);
            knots.push_back(0.44);
            knots.push_back(0.48);
            knots.push_back(0.5);

            // empty arg list for coefficients
            RooArgList* list = new RooArgList();

            // create first coefficient
            RooRealVar* coeff_first = &(epdf->Var("parCSpline1"));
            coeff_first->setRange(0,10000);
            coeff_first->setVal(1);
            coeff_first->setConstant(false);
            list->add( *coeff_first );

            for (unsigned int i=1; i <= knots.size(); ++i){
               std::string number = boost::lexical_cast<std::string>(i);
               RooRealVar* coeff = &(epdf->Var("parCSpline"+number));
               coeff->setRange(0,10000);
               coeff->setVal(1);
               coeff->setConstant(false);
               list->add( *coeff );
            }

            // create last coefficient
            RooRealVar* coeff_last = &(epdf->Var("parCSpline"+boost::lexical_cast<std::string>(knots.size())));
            coeff_last->setRange(0,10000);
            coeff_last->setVal(1);
            coeff_last->setConstant(false);
            list->add( *coeff_last );



            list->Print();

            doofit::roofit::pdfs::DooCubicSplinePdf splinePdf("splinePdf",epdf->Var("obsEtaOS"),knots,*list,0,0.5);
            //doofit::roofit::pdfs::DooCubicSplinePdf* splinePdf = new doofit::roofit::pdfs::DooCubicSplinePdf("splinePdf", epdf->Var("obsEtaOS"), knots, *list,0,0.5);



     //Koeffizienten
            DecRateCoeff *coeff_c = new DecRateCoeff("coef_cos","coef_cos",DecRateCoeff::CPOdd,epdf->Cat("catFinalState"),epdf->Cat("obsTag"),epdf->Var("C_f"),epdf->Var("C_fbar"),epdf->Var("obsEtaOS"),splinePdf,epdf->Var("tageff"),epdf->Var("obsEtaOS"),epdf->Var("asym_prod"),epdf->Var("asym_det"),epdf->Var("asym_tageff"));
            DecRateCoeff *coeff_s = new DecRateCoeff("coef_sin","coef_sin",DecRateCoeff::CPOdd,epdf->Cat("catFinalState"),epdf->Cat("obsTag"),epdf->Var("S_f"),epdf->Var("S_fbar"),epdf->Var("obsEtaOS"),splinePdf,epdf->Var("tageff"),epdf->Var("obsEtaOS"),epdf->Var("asym_prod"),epdf->Var("asym_det"),epdf->Var("asym_tageff"));
            DecRateCoeff *coeff_sh = new DecRateCoeff("coef_sinh","coef_sinh",DecRateCoeff::CPEven,epdf->Cat("catFinalState"),epdf->Cat("obsTag"),epdf->Var("f1_f"),epdf->Var("f1_fbar"),epdf->Var("obsEtaOS"),splinePdf,epdf->Var("tageff"),epdf->Var("obsEtaOS"),epdf->Var("asym_prod"),epdf->Var("asym_det"),epdf->Var("asym_tageff"));
            DecRateCoeff *coeff_ch = new DecRateCoeff("coef_cosh","coef_cosh",DecRateCoeff::CPEven,epdf->Cat("catFinalState"),epdf->Cat("obsTag"),epdf->Var("f0_f"),epdf->Var("f0_fbar"),epdf->Var("obsEtaOS"),splinePdf,epdf->Var("tageff"),epdf->Var("obsEtaOS"),epdf->Var("asym_prod"),epdf->Var("asym_det"),epdf->Var("asym_tageff"));

            epdf->AddRealToStore(coeff_ch);
            epdf->AddRealToStore(coeff_sh);
            epdf->AddRealToStore(coeff_c);
            epdf->AddRealToStore(coeff_s);


    ///////////////////Generiere PDF's/////////////////////
    //Zeit
    epdf->GaussModel("resTimeGauss",epdf->Var("obsTime"),epdf->Var("allTimeResMean"),epdf->Var("allTimeReso"));
    epdf->BDecay("pdfSigTime",epdf->Var("obsTime"),epdf->Var("tau"),epdf->Var("dgamma"),epdf->Real("coef_cosh"),epdf->Real("coef_sinh"),epdf->Real("coef_cos"),epdf->Real("coef_sin"),epdf->Var("deltaM"),epdf->Model("resTimeGauss"));


     //Zusammenfassen der Parameter in einem RooArgSet
     RooArgSet Observables;
     Observables.add(RooArgSet( epdf->Var("obsTime"),epdf->Cat("catFinalState"),epdf->Cat("obsTag"),epdf->Var("obsEtaOS")));


     epdf->Extend("pdfExtend", epdf->Pdf("pdfSigTime"),epdf->Real("sig_yield"));



     //Multipliziere Signal und Untergrund PDF mit ihrer jeweiligen Zerfalls PDF//
     //Untergrund * Zerfall
     /*epdf->Product("pdf_bkg", RooArgSet(epdf->Pdf("pdf_bkg_mass_expo"), epdf->Pdf("pdf_bkg_mass_time")));
     //Signal * Zerfall
     epdf->Product("pdf_sig", RooArgSet(epdf->Pdf("pdf_sig_mass_gauss"),epdf->Pdf("pdfSigTime")));
    //Addiere PDF's
     epdf->Add("pdf_total", RooArgSet(epdf->Pdf("pdf_sig_mass_gauss*pdf_sig_time_decay"), epdf->Pdf("pdf_bkg_mass*pdf_bkg_time_decay")), RooArgSet(epdf->Var("bkg_Yield"),epdf->Var("sig_Yield")));*/





     RooWorkspace ws;
                 ws.import(epdf->Pdf("pdfExtend"));
                 ws.defineSet("Observables",Observables, true);

                 ws.Print();

                 doofit::config::CommonConfig cfg_com("common");
                 cfg_com.InitializeOptions(argc, argv);
                 doofit::toy::ToyFactoryStdConfig cfg_tfac("toyfac");
                 cfg_tfac.InitializeOptions(cfg_com);
                 doofit::toy::ToyStudyStdConfig cfg_tstudy("toystudy");
                 cfg_tstudy.InitializeOptions(cfg_tfac);

                 // set a previously defined workspace to get PDF from (not mandatory, but convenient)
                 cfg_tfac.set_workspace(&ws);

                 // Check for a set --help flag and if so, print help and exit gracefully
                 // (recommended).
                 cfg_com.CheckHelpFlagAndPrintHelp();

                 // More custom code, e.g. to set options internally.
                 // Not required as configuration via command line/config file is enough.
                 cfg_com.PrintAll();

                 // Print overview of all options (optional)
                 // cfg_com.PrintAll();

                 // Initialize the toy factory module with the config objects and start
                 // generating toy samples.
                 doofit::toy::ToyFactoryStd tfac(cfg_com, cfg_tfac);
                 doofit::toy::ToyStudyStd tstudy(cfg_com, cfg_tstudy);




          RooDataSet* data = tfac.Generate();
          data->Print();
          epdf->Pdf("pdfExtend").getParameters(data)->readFromFile("/home/chasenberg/Repository/bachelor-template/ToyStudy/dootoycp-parameter_spline.txt");
          epdf->Pdf("pdfExtend").getParameters(data)->writeToFile("/home/chasenberg/Repository/bachelor-template/ToyStudy/dootoycp-parameter_spline.txt.new");

          //epdf->Pdf("pdfExtend").fitTo(*data);
          //epdf->Pdf("pdfExtend").getParameters(data)->writeToFile("/home/chasenberg/Repository/bachelor-template/ToyStudy/dootoycp-fit-result.txt");
		      RooFitResult* fit_result = epdf->Pdf("pdfExtend").fitTo(*data, RooFit::Save(true));
       	  tstudy.StoreFitResult(fit_result);

          /*using namespace doofit::plotting;

          PlotConfig cfg_plot("cfg_plot");
          cfg_plot.InitializeOptions();
          cfg_plot.set_plot_directory("/net/lhcb-tank/home/chasenberg/Ergebnis/dootoycp_spline-lhcb/time/");
          // plot PDF and directly specify components
          Plot myplot(cfg_plot, epdf->Var("obsTime"), *data, RooArgList(epdf->Pdf("pdfExtend")));
          myplot.PlotItLogNoLogY();

          PlotConfig cfg_plotEta("cfg_plotEta");
          cfg_plotEta.InitializeOptions();
          cfg_plotEta.set_plot_directory("/net/lhcb-tank/home/chasenberg/Ergebnis/dootoycp_spline-lhcb/eta/");
          // plot PDF and directly specify components
          Plot myplotEta(cfg_plotEta, epdf->Var("obsEtaOS"), *data, RooArgList(splinePdf));
          myplotEta.PlotIt();*/

 }