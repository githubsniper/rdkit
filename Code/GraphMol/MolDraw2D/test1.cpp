//
//  Copyright (C) 2015 Greg Landrum
//
//   @@ All Rights Reserved @@
//  This file is part of the RDKit.
//  The contents are covered by the terms of the BSD license
//  which is included in the file license.txt, found at the root
//  of the RDKit source tree.
//

#include <RDGeneral/utils.h>
#include <RDGeneral/Invariant.h>
#include <RDGeneral/RDLog.h>
#include <GraphMol/RDKitBase.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/FileParsers/MolSupplier.h>
#include <GraphMol/FileParsers/FileParsers.h>
#include <GraphMol/Depictor/RDDepictor.h>
#include <GraphMol/FileParsers/MolFileStereochem.h>

#include <GraphMol/MolDraw2D/MolDraw2D.h>
#include <GraphMol/MolDraw2D/MolDraw2DSVG.h>
#include <GraphMol/MolDraw2D/MolDraw2DUtils.h>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace RDKit;

void test1() {
  std::cout << " ----------------- Test 1" << std::endl;
  {
    std::string smiles = "CO[C@@H](O)C1=C(O[C@H](F)Cl)C(C#N)=C1ONNC[NH3+]";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));
    std::ofstream outs("test1_1.svg");
    MolDraw2DSVG drawer(300, 300, outs);
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    outs.flush();
    delete m;
  }
  {
    // make sure this works with the stringstream too:
    std::string smiles = "CO[C@@H](O)C1=C(O[C@H](F)Cl)C=C1ONNC[NH3+]";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));
    MolDraw2DSVG drawer(300, 300);
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    std::string text = drawer.getDrawingText();
    TEST_ASSERT(text.find("<svg:svg") != std::string::npos);
    TEST_ASSERT(text.find("</svg:svg>") != std::string::npos);
    delete m;
  }
  {
    std::string smiles = "Cc1c(C(=O)NCCO)[n+](=O)c2ccccc2n1[O-]";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));
    std::ofstream outs("test1_2.svg");
    MolDraw2DSVG drawer(300, 300, outs);
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    outs.flush();
    delete m;
  }
  {
    std::string smiles = "Cc1c(C(=O)NCCO)[n+](=O)c2ccccc2n1[O-]";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));
    std::ofstream outs("test1_3.svg");
    MolDraw2DSVG drawer(300, 300, outs);
    std::vector<int> highlights;
    highlights.push_back(0);
    highlights.push_back(4);
    highlights.push_back(5);
    drawer.drawMolecule(*m, &highlights);
    drawer.finishDrawing();
    outs.flush();
    delete m;
  }
  std::cout << " Done" << std::endl;
}

#ifdef RDK_CAIRO_BUILD
#include <cairo.h>
#include "MolDraw2DCairo.h"
void test2() {
  std::cout << " ----------------- Test 2" << std::endl;
  {
    std::string smiles = "CO[C@@H](O)C1=C(O[C@H](F)Cl)C(C#N)=C1ONNC[NH3+]";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));

    MolDraw2DCairo drawer(300, 300);
    drawer.drawMolecule(*m);
    drawer.finishDrawing();

    drawer.writeDrawingText("test2_1.png");
    delete m;
  }
  {
    std::string smiles = "Cc1c(C(=O)NCCO)[n+](=O)c2ccccc2n1[O-]";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));

    MolDraw2DCairo drawer(300, 300);
    drawer.drawMolecule(*m);
    drawer.finishDrawing();

    std::string drawing = drawer.getDrawingText();
    std::ofstream ofs("test2_2.png");
    ofs.write(drawing.c_str(), drawing.size());
    delete m;
  }
  {
    // ensure we still work with a client-provided drawing context
    std::string smiles = "Cc1c(C(=O)NCCO)[n+](=O)c2ccccc2n1[O-]";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));

    cairo_surface_t *surface =
        cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 300, 300);
    cairo_t *cr = cairo_create(surface);

    MolDraw2DCairo drawer(300, 300, cr);
    std::vector<int> highlights;
    highlights.push_back(0);
    highlights.push_back(4);
    highlights.push_back(5);
    drawer.drawMolecule(*m, &highlights);
    drawer.finishDrawing();

    cairo_destroy(cr);
    cairo_surface_write_to_png(surface, "test2_3.png");
    cairo_surface_destroy(surface);

    delete m;
  }
  std::cout << " Done" << std::endl;
}
#else  // RDK_CAIRO_BUILD
void test2() {}
#endif

void test3() {
  std::cout << " ----------------- Test 3" << std::endl;
  {
    std::string smiles = "C1CC1CC1ON1";
    std::string nameBase = "test3_1";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));
    static const int ha[] = {0, 3, 4, 5};
    std::vector<int> highlight_atoms(ha, ha + sizeof(ha) / sizeof(int));
    std::map<int, std::string> atomLabels;
    atomLabels[2] = "C1";
    atomLabels[1] = "a<sub>3</sub><sup>4</sup>";
    atomLabels[0] = "[CH2;X2:4]";
    atomLabels[6] = "[NH2+:7]";

#ifdef RDK_CAIRO_BUILD
    {
      MolDraw2DCairo drawer(300, 300);
      drawer.drawOptions().atomLabels = atomLabels;
      drawer.drawMolecule(*m, &highlight_atoms);
      drawer.finishDrawing();

      drawer.writeDrawingText(nameBase + ".png");
    }
#endif
    {
      std::ofstream outs((nameBase + ".svg").c_str());
      MolDraw2DSVG drawer(300, 300, outs);
      drawer.drawOptions().atomLabels = atomLabels;
      drawer.drawMolecule(*m, &highlight_atoms);
      drawer.finishDrawing();
      outs.flush();
    }
    delete m;
  }

  {
    std::string smiles = "C1CC1CC1ON1";
    std::string nameBase = "test3_2";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));
    static const int ha[] = {0, 3, 4, 5};
    std::vector<int> highlight_atoms(ha, ha + sizeof(ha) / sizeof(int));

#ifdef RDK_CAIRO_BUILD
    {
      MolDraw2DCairo drawer(300, 300);
      drawer.drawOptions().circleAtoms = false;
      drawer.drawMolecule(*m, &highlight_atoms);
      drawer.finishDrawing();
      drawer.writeDrawingText(nameBase + ".png");
    }
#endif
    {
      std::ofstream outs((nameBase + ".svg").c_str());
      MolDraw2DSVG drawer(300, 300, outs);
      drawer.drawOptions().circleAtoms = false;
      drawer.drawMolecule(*m, &highlight_atoms);
      drawer.finishDrawing();
      outs.flush();
    }
    delete m;
  }
  {
    std::string smiles = "Cc1c(C(=O)NCCO)[n+](=O)c2ccccc2n1[O-]";
    std::string nameBase = "test3_3";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));
    static const int ha[] = {11, 12, 13, 14, 15, 16};
    std::vector<int> highlight_atoms(ha, ha + sizeof(ha) / sizeof(int));
    std::map<int, DrawColour> highlight_colors;
    highlight_colors[12] = DrawColour(0, 0, 1);
    highlight_colors[13] = DrawColour(0, 1, 0);

#ifdef RDK_CAIRO_BUILD
    {
      MolDraw2DCairo drawer(300, 300);
      drawer.drawOptions().circleAtoms = true;
      drawer.drawMolecule(*m, &highlight_atoms, &highlight_colors);
      drawer.finishDrawing();
      drawer.writeDrawingText(nameBase + ".png");
    }
#endif
    {
      std::ofstream outs((nameBase + ".svg").c_str());
      MolDraw2DSVG drawer(300, 300, outs);
      drawer.drawOptions().circleAtoms = true;
      drawer.drawMolecule(*m, &highlight_atoms, &highlight_colors);
      drawer.finishDrawing();
      outs.flush();
    }
    delete m;
  }
  {
    std::string smiles = "Cc1c(C(=O)NCCO)[n+](=O)c2ccccc2n1[O-]";
    std::string nameBase = "test3_4";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));
    static const int ha[] = {11, 12, 13, 14, 15, 16, 3};
    std::vector<int> highlight_atoms(ha, ha + sizeof(ha) / sizeof(int));
    std::map<int, DrawColour> highlight_colors;
    highlight_colors[12] = DrawColour(.5, .5, 1);
    highlight_colors[13] = DrawColour(.5, 1, .5);
    MolDrawOptions options;
    options.circleAtoms = true;
    options.highlightColour = DrawColour(1, .5, .5);
    options.continuousHighlight = true;

#ifdef RDK_CAIRO_BUILD
    {
      MolDraw2DCairo drawer(300, 300);
      drawer.drawOptions() = options;
      drawer.drawMolecule(*m, &highlight_atoms, &highlight_colors);
      drawer.finishDrawing();
      drawer.writeDrawingText(nameBase + ".png");
    }
#endif
    {
      std::ofstream outs((nameBase + ".svg").c_str());
      MolDraw2DSVG drawer(300, 300, outs);
      drawer.drawOptions() = options;
      drawer.drawMolecule(*m, &highlight_atoms, &highlight_colors);
      drawer.finishDrawing();
      outs.flush();
    }
    delete m;
  }
  {
    std::string smiles =
        "CCOC(=O)Nc1ccc(SCC2COC(Cn3ccnc3)(c3ccc(Cl)cc3Cl)O2)cc1";
    std::string nameBase = "test3_5";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));

    static const int ha[] = {17, 18, 19, 20, 21, 6, 7, 8, 9, 31, 32};
    std::vector<int> highlight_atoms(ha, ha + sizeof(ha) / sizeof(int));
    std::map<int, DrawColour> highlight_colors;
    MolDrawOptions options;
    options.circleAtoms = true;
    options.highlightColour = DrawColour(1, .5, .5);
    options.continuousHighlight = true;

#ifdef RDK_CAIRO_BUILD
    {
      MolDraw2DCairo drawer(200, 200);
      drawer.drawOptions() = options;
      drawer.drawMolecule(*m, &highlight_atoms, &highlight_colors);
      drawer.finishDrawing();
      drawer.writeDrawingText(nameBase + ".png");
    }
#endif
    {
      std::ofstream outs((nameBase + ".svg").c_str());
      MolDraw2DSVG drawer(200, 200, outs);
      drawer.drawOptions() = options;
      drawer.drawMolecule(*m, &highlight_atoms, &highlight_colors);
      drawer.finishDrawing();
      outs.flush();
    }
    delete m;
  }

  {
    std::string smiles =
        "CCOC(=O)Nc1ccc(SCC2COC(Cn3ccnc3)(c3ccc(Cl)cc3Cl)O2)cc1";
    std::string nameBase = "test3_6";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));

    MolDrawOptions options;
    static const int ha1[] = {17, 18, 19, 20, 21};
    std::vector<int> highlight_atoms1(ha1, ha1 + sizeof(ha1) / sizeof(int));
    options.atomRegions.push_back(highlight_atoms1);
    static const int ha2[] = {6, 7, 8, 9, 31, 32};
    std::vector<int> highlight_atoms2(ha2, ha2 + sizeof(ha2) / sizeof(int));
    options.atomRegions.push_back(highlight_atoms2);
    options.includeAtomTags = true;

#ifdef RDK_CAIRO_BUILD
    {
      MolDraw2DCairo drawer(300, 300);
      drawer.drawOptions() = options;
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      drawer.writeDrawingText(nameBase + ".png");
    }
#endif
    {
      std::ofstream outs((nameBase + ".svg").c_str());
      MolDraw2DSVG drawer(300, 300, outs);
      drawer.drawOptions() = options;
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      outs.flush();
    }
    delete m;
  }
  {
    std::string smiles =
        "CCOC(=O)Nc1ccc(SCC2COC(Cn3ccnc3)(c3ccc(Cl)cc3Cl)O2)cc1";
    std::string nameBase = "test3_7";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));

    MolDrawOptions options;
    options.continuousHighlight = true;
    static const int ha[] = {17, 20, 25};
    std::vector<int> highlight_atoms(ha, ha + sizeof(ha) / sizeof(int));
    std::map<int, double> highlight_radii;
    highlight_radii[17] = 0.5;
    highlight_radii[20] = 1.0;
    std::map<int, DrawColour> highlight_colors;
    highlight_colors[17] = DrawColour(.5, .5, 1.);

#ifdef RDK_CAIRO_BUILD
    {
      MolDraw2DCairo drawer(300, 300);
      drawer.drawOptions() = options;
      drawer.drawMolecule(*m, &highlight_atoms, &highlight_colors,
                          &highlight_radii);
      drawer.finishDrawing();
      drawer.writeDrawingText(nameBase + ".png");
    }
#endif
    {
      std::ofstream outs((nameBase + ".svg").c_str());
      MolDraw2DSVG drawer(300, 300, outs);
      drawer.drawOptions() = options;
      drawer.drawMolecule(*m, &highlight_atoms, &highlight_colors,
                          &highlight_radii);
      drawer.finishDrawing();
      outs.flush();
    }
    delete m;
  }

  std::cout << " Done" << std::endl;
}

void test4() {
  std::cout << " ----------------- Test 4" << std::endl;
  {
    std::string fName = getenv("RDBASE");
    fName += "/Code/GraphMol/MolDraw2D/test_dir";
    fName += "/clash.mol";
    ROMol *m = MolFileToMol(fName);
    std::string nameBase = "test4_1";
    TEST_ASSERT(m);

#ifdef RDK_CAIRO_BUILD
    {
      MolDraw2DCairo drawer(300, 300);
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      drawer.writeDrawingText(nameBase + ".png");
    }
#endif
    {
      std::ofstream outs((nameBase + ".svg").c_str());
      MolDraw2DSVG drawer(300, 300, outs);
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      outs.flush();
    }
    delete m;
  }

  std::cout << " Done" << std::endl;
}

void test5() {
  std::cout << " ----------------- Test 5" << std::endl;
  {
    std::string smiles = "*c1cc(*)cc(*)c1";
    std::string nameBase = "test5_1";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));
    MolDrawOptions options;
    options.dummiesAreAttachments = true;
    options.atomLabels[0] = "R1";
#ifdef RDK_CAIRO_BUILD
    {
      MolDraw2DCairo drawer(300, 300);
      drawer.drawOptions() = options;
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      drawer.writeDrawingText(nameBase + ".png");
    }
#endif
    {
      std::ofstream outs((nameBase + ".svg").c_str());
      MolDraw2DSVG drawer(300, 300, outs);
      drawer.drawOptions() = options;
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      outs.flush();
    }
    delete m;
  }
  {
    std::string smiles = "*C";
    std::string nameBase = "test5_2";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m, 0, true);
    WedgeMolBonds(*m, &(m->getConformer()));
    MolDrawOptions options;
    options.dummiesAreAttachments = true;
#ifdef RDK_CAIRO_BUILD
    {
      MolDraw2DCairo drawer(300, 300);
      drawer.drawOptions() = options;
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      drawer.writeDrawingText(nameBase + ".png");
    }
#endif
    {
      std::ofstream outs((nameBase + ".svg").c_str());
      MolDraw2DSVG drawer(300, 300, outs);
      drawer.drawOptions() = options;
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      outs.flush();
    }
    delete m;
  }
  {
    std::string smiles = "CC(F)(Cl)Br";
    std::string nameBase = "test5_3";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    m->getBondBetweenAtoms(1, 2)->setBondDir(Bond::UNKNOWN);
    RDDepict::compute2DCoords(*m, 0, true);
    WedgeMolBonds(*m, &(m->getConformer()));
    MolDrawOptions options;
    options.dummiesAreAttachments = true;
#ifdef RDK_CAIRO_BUILD
    {
      MolDraw2DCairo drawer(300, 300);
      drawer.drawOptions() = options;
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      drawer.writeDrawingText(nameBase + ".png");
    }
#endif
    {
      std::ofstream outs((nameBase + ".svg").c_str());
      MolDraw2DSVG drawer(300, 300, outs);
      drawer.drawOptions() = options;
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      outs.flush();
    }
    delete m;
  }
  std::cout << " Done" << std::endl;
}

#ifdef RDK_TEST_MULTITHREADED
#include <RDGeneral/BoostStartInclude.h>
#include <boost/thread.hpp>
#include <RDGeneral/BoostEndInclude.h>
namespace {
void runblock(const std::vector<ROMol *> &mols,
              const std::vector<std::string> &refData, unsigned int count,
              unsigned int idx) {
  for (unsigned int j = 0; j < 200; j++) {
    for (unsigned int i = 0; i < mols.size(); ++i) {
      if (i % count != idx) continue;
      ROMol *mol = mols[i];
      MolDraw2DSVG drawer(300, 300);
      drawer.drawMolecule(*mol);
      drawer.finishDrawing();
      std::string text = drawer.getDrawingText();
      TEST_ASSERT(text == refData[i]);
    }
  }
}
}

void testMultiThreaded() {
  std::cout << " ----------------- Test multi-threaded drawing" << std::endl;
  std::string fName = getenv("RDBASE");
  fName += "/Data/NCI/first_200.props.sdf";
  RDKit::SDMolSupplier suppl(fName);
  std::cerr << "reading molecules" << std::endl;
  std::vector<ROMol *> mols;
  while (!suppl.atEnd() && mols.size() < 100) {
    ROMol *mol = 0;
    try {
      mol = suppl.next();
    } catch (...) {
      continue;
    }
    if (!mol) continue;
    mols.push_back(mol);
  }

  std::cerr << "generating reference drawings" << std::endl;
  std::vector<std::string> refData(mols.size());
  for (unsigned int i = 0; i < mols.size(); ++i) {
    MolDraw2DSVG drawer(300, 300);
    drawer.drawMolecule(*(mols[i]));
    drawer.finishDrawing();
    refData[i] = drawer.getDrawingText();
    TEST_ASSERT(refData[i].find("<svg:svg") != std::string::npos);
    TEST_ASSERT(refData[i].find("</svg:svg>") != std::string::npos);
  }

  boost::thread_group tg;
  unsigned int count = 4;
  std::cerr << "processing" << std::endl;
  for (unsigned int i = 0; i < count; ++i) {
    std::cerr << " launch :" << i << std::endl;
    std::cerr.flush();
    tg.add_thread(new boost::thread(runblock, mols, refData, count, i));
  }
  tg.join_all();
  std::cerr << " Done" << std::endl;
}
#else
void testMultiThreaded() {}
#endif

void test6() {
  std::cout << " ----------------- Test 6 (atom labels)" << std::endl;
  {
    std::string smiles = "CC[13CH2][CH2:7][CH-]C[15NH2+]C";
    std::string nameBase = "test5_1";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));
    MolDraw2DSVG drawer(300, 300);
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    std::string txt = drawer.getDrawingText();
    std::ofstream outs("test6_1.svg");
    outs << txt;
    // TEST_ASSERT(txt.find("<svg:svg")!=std::string::npos);
  }
  std::cerr << " Done" << std::endl;
}

void test7() {
  std::cout << " ----------------- Test 7 (backgrounds)" << std::endl;
  std::string smiles = "CCC";
  ROMol *m = SmilesToMol(smiles);
  TEST_ASSERT(m);
  RDDepict::compute2DCoords(*m);
  {
    std::string nameBase = "test7_1";
    MolDraw2DSVG drawer(300, 300);
    drawer.drawOptions().clearBackground = false;
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    std::string txt = drawer.getDrawingText();
    std::ofstream outs((nameBase + ".svg").c_str());
    outs << txt;
    TEST_ASSERT(txt.find("<svg:svg") != std::string::npos);
    TEST_ASSERT(txt.find("<svg:rect") == std::string::npos);
  }
#ifdef RDK_CAIRO_BUILD
  {
    std::string nameBase = "test7_1";
    MolDraw2DCairo drawer(300, 300);
    drawer.drawOptions().clearBackground = false;
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    drawer.writeDrawingText(nameBase + ".png");
  }
#endif
  {
    std::string nameBase = "test7_2";
    MolDraw2DSVG drawer(300, 300);
    drawer.drawOptions().backgroundColour = DrawColour(.8, .8, .8);
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    std::string txt = drawer.getDrawingText();
    std::ofstream outs((nameBase + ".svg").c_str());
    outs << txt;
    TEST_ASSERT(txt.find("<svg:svg") != std::string::npos);
    TEST_ASSERT(txt.find("<svg:rect") != std::string::npos);
    TEST_ASSERT(txt.find("fill:#CCCCCC") != std::string::npos);
  }
#ifdef RDK_CAIRO_BUILD
  {
    std::string nameBase = "test7_2";
    MolDraw2DCairo drawer(300, 300);
    drawer.drawOptions().backgroundColour = DrawColour(.8, .8, .8);
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    drawer.writeDrawingText(nameBase + ".png");
  }
#endif
  std::cerr << " Done" << std::endl;
}

void test8PrepareMolForDrawing() {
  std::cout << " ----------------- Test8: PrepareMolDrawing" << std::endl;
  {
    std::string smiles = "c1ccccc1[C@H](F)Cl";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    {
      RWMol nm(*m);
      TEST_ASSERT(nm.getNumAtoms() == 9)
      MolDraw2DUtils::prepareMolForDrawing(nm);
      TEST_ASSERT(nm.getNumAtoms() == 9);  // this is a test for github #982
      TEST_ASSERT(nm.getNumConformers() == 1);
      TEST_ASSERT(!nm.getConformer().is3D());
      TEST_ASSERT(nm.getBondBetweenAtoms(0, 1)->getBondType() !=
                  Bond::AROMATIC);
      TEST_ASSERT(nm.getBondBetweenAtoms(0, 1)->getIsAromatic());
      TEST_ASSERT(nm.getBondBetweenAtoms(6, 7)->getBondType() == Bond::SINGLE);
      TEST_ASSERT(nm.getBondBetweenAtoms(6, 7)->getBondDir() ==
                  Bond::BEGINWEDGE);

      // make sure we can do it again:
      MolDraw2DUtils::prepareMolForDrawing(nm);
      TEST_ASSERT(nm.getNumAtoms() == 9);
      TEST_ASSERT(nm.getNumConformers() == 1);
      TEST_ASSERT(nm.getBondBetweenAtoms(0, 1)->getBondType() !=
                  Bond::AROMATIC);
      TEST_ASSERT(nm.getBondBetweenAtoms(0, 1)->getIsAromatic());
    }
    {
      RWMol nm(*m);
      TEST_ASSERT(nm.getNumAtoms() == 9)
      MolDraw2DUtils::prepareMolForDrawing(nm, false);
      TEST_ASSERT(nm.getNumAtoms() == 9);
      TEST_ASSERT(nm.getNumConformers() == 1);
      TEST_ASSERT(nm.getBondBetweenAtoms(0, 1)->getBondType() ==
                  Bond::AROMATIC);
      TEST_ASSERT(nm.getBondBetweenAtoms(0, 1)->getIsAromatic());
    }
    {
      RWMol nm(*m);
      TEST_ASSERT(nm.getNumAtoms() == 9)
      MolDraw2DUtils::prepareMolForDrawing(nm, false, false);
      TEST_ASSERT(nm.getNumAtoms() == 9);
      TEST_ASSERT(nm.getNumConformers() == 1);
      TEST_ASSERT(nm.getBondBetweenAtoms(0, 1)->getBondType() ==
                  Bond::AROMATIC);
      TEST_ASSERT(nm.getBondBetweenAtoms(0, 1)->getIsAromatic());
    }
    {
      RWMol nm(*m);
      TEST_ASSERT(nm.getNumAtoms() == 9)
      MolDraw2DUtils::prepareMolForDrawing(nm, false, true);
      TEST_ASSERT(nm.getNumAtoms() == 9);
      TEST_ASSERT(nm.getNumConformers() == 1);
      TEST_ASSERT(nm.getBondBetweenAtoms(0, 1)->getBondType() ==
                  Bond::AROMATIC);
      TEST_ASSERT(nm.getBondBetweenAtoms(0, 1)->getIsAromatic());
    }

    {
      RWMol nm(*m);
      TEST_ASSERT(nm.getNumAtoms() == 9)
      MolDraw2DUtils::prepareMolForDrawing(nm, true, true, false);
      TEST_ASSERT(nm.getNumAtoms() == 9);
      TEST_ASSERT(nm.getNumConformers() == 1);
      TEST_ASSERT(nm.getBondBetweenAtoms(0, 1)->getBondType() !=
                  Bond::AROMATIC);
      TEST_ASSERT(nm.getBondBetweenAtoms(0, 1)->getIsAromatic());
      TEST_ASSERT(nm.getBondBetweenAtoms(6, 7)->getBondType() == Bond::SINGLE);
      TEST_ASSERT(nm.getBondBetweenAtoms(6, 7)->getBondDir() == Bond::NONE);
    }

    {
      // by default we don't force conformer generation
      RWMol nm(*m);
      RDDepict::compute2DCoords(nm);
      nm.getConformer().set3D(true);  // it's not really, we're cheating
      TEST_ASSERT(nm.getNumAtoms() == 9)
      MolDraw2DUtils::prepareMolForDrawing(nm);
      TEST_ASSERT(nm.getNumAtoms() == 9);
      TEST_ASSERT(nm.getNumConformers() == 1);  // we have a conformer anyway
      TEST_ASSERT(nm.getConformer().is3D());

      // but if we do force, it blows out that conformer:
      MolDraw2DUtils::prepareMolForDrawing(nm, true, true, true, true);
      TEST_ASSERT(!nm.getConformer().is3D());
    }

    delete m;
  }
  {
    std::string smiles = "C1CC[C@H]2NCCCC2C1";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    {
      RWMol nm(*m);
      TEST_ASSERT(nm.getNumAtoms() == 10)
      MolDraw2DUtils::prepareMolForDrawing(nm);
      TEST_ASSERT(nm.getNumAtoms() == 11);
      TEST_ASSERT(nm.getNumConformers() == 1);
      TEST_ASSERT(!nm.getConformer().is3D());
      TEST_ASSERT(nm.getBondBetweenAtoms(3, 10)->getBondType() == Bond::SINGLE);
      TEST_ASSERT(nm.getBondBetweenAtoms(3, 10)->getBondDir() ==
                  Bond::BEGINDASH);

      // make sure we can do it again:
      MolDraw2DUtils::prepareMolForDrawing(nm);
      TEST_ASSERT(nm.getNumAtoms() == 11);
      TEST_ASSERT(nm.getNumConformers() == 1);
      TEST_ASSERT(nm.getBondBetweenAtoms(3, 10)->getBondType() == Bond::SINGLE);
      TEST_ASSERT(nm.getBondBetweenAtoms(3, 10)->getBondDir() ==
                  Bond::BEGINDASH);
    }
    {
      RWMol nm(*m);
      TEST_ASSERT(nm.getNumAtoms() == 10)
      MolDraw2DUtils::prepareMolForDrawing(nm, false, false);
      TEST_ASSERT(nm.getNumAtoms() == 10);
      TEST_ASSERT(nm.getNumConformers() == 1);
      TEST_ASSERT(nm.getBondBetweenAtoms(3, 2)->getBondType() == Bond::SINGLE);
      TEST_ASSERT(nm.getBondBetweenAtoms(3, 2)->getBondDir() ==
                  Bond::BEGINWEDGE);
    }
    delete m;
  }
  std::cerr << " Done" << std::endl;
}

void testGithub781() {
  std::cout << " ----------------- Test Github #781: Rendering single-atom "
               "molecules"
            << std::endl;

  {
    std::string smiles = "C";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    MolDraw2DSVG drawer(300, 300);
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    std::string txt = drawer.getDrawingText();
    TEST_ASSERT(txt.find("<svg:svg") != std::string::npos);
    TEST_ASSERT(txt.find("<svg:tspan>CH</svg:tspan>") != std::string::npos);
    delete m;
  }
  {
    std::string smiles = "[C]";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    MolDraw2DSVG drawer(300, 300);
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    std::string txt = drawer.getDrawingText();
    TEST_ASSERT(txt.find("<svg:svg") != std::string::npos);
    TEST_ASSERT(txt.find("<svg:tspan>C</svg:tspan>") != std::string::npos);
    delete m;
  }
  {
    std::string smiles = "C.CC.[Cl-]";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    MolDraw2DSVG drawer(300, 300);
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    std::string txt = drawer.getDrawingText();
    TEST_ASSERT(txt.find("<svg:svg") != std::string::npos);
    TEST_ASSERT(txt.find("<svg:tspan>CH</svg:tspan>") != std::string::npos);
    TEST_ASSERT(txt.find("<svg:tspan>Cl</svg:tspan>") != std::string::npos);
    delete m;
  }
  {  // empty molecule
    ROMol *m = new ROMol();
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    MolDraw2DSVG drawer(300, 300);
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    std::string txt = drawer.getDrawingText();
    TEST_ASSERT(txt.find("<svg:svg") != std::string::npos);
    TEST_ASSERT(txt.find("<svg:tspan>") == std::string::npos);
    delete m;
  }
  std::cerr << " Done" << std::endl;
}

void testGithub774() {
  std::cout << " ----------------- Test Github774" << std::endl;
  {
    std::string smiles =
        "Cc1c(C(=O)NCC[NH3+])[n+](=O)c2cc(CC[C@](F)(Cl)Br)ccc2n1[O-]";
    std::string nameBase = "test774_1";
    RWMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));
    MolOps::Kekulize(*m);

#ifdef RDK_CAIRO_BUILD
    {
      MolDraw2DCairo drawer(300, 300);
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      drawer.writeDrawingText(nameBase + ".png");
    }
#endif
    {
      std::ofstream outs((nameBase + ".svg").c_str());
      MolDraw2DSVG drawer(300, 300, outs);
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      outs.flush();
      Point2D ocoords(1.0, 2.0);
      Point2D dcoords =
          drawer.getAtomCoords(std::make_pair(ocoords.x, ocoords.y));
      Point2D acoords = drawer.getDrawCoords(dcoords);
      TEST_ASSERT(feq(acoords.x, 1.0));
      TEST_ASSERT(feq(acoords.y, 2.0));
    }
    // m->setProp("_Name","mol");
    // std::cerr<<MolToMolBlock(*m)<<std::endl;
    delete m;
  }
  {
    std::string smiles =
        "CC(=O)\\C=C\\CC1[C@H]2N([C@@H](C(=O)O)C(C)(C)S2(=O)=O)C1=O";
    std::string nameBase = "test774_2";
    RWMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));
    MolOps::Kekulize(*m);

#ifdef RDK_CAIRO_BUILD
    {
      MolDraw2DCairo drawer(300, 300);
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      drawer.writeDrawingText(nameBase + ".png");
    }
#endif
    {
      std::ofstream outs((nameBase + ".svg").c_str());
      MolDraw2DSVG drawer(300, 300, outs);
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      outs.flush();
    }
    // m->setProp("_Name","mol");
    // std::cerr<<MolToMolBlock(*m)<<std::endl;
    delete m;
  }
  std::cerr << " Done" << std::endl;
}

void test9MolLegends() {
  std::cout << " ----------------- Test 9 (molecule legends)" << std::endl;
  {
    std::string smiles = "CC[13CH2][CH2:7][CH-]C[15NH2+]C";
    std::string nameBase = "test5_1";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));
    MolDraw2DSVG drawer(300, 300);
    drawer.drawMolecule(*m, "mol legend");
    drawer.finishDrawing();
    std::string txt = drawer.getDrawingText();
    std::ofstream outs("test9_1.svg");
    outs << txt;
    // TEST_ASSERT(txt.find("<svg:svg")!=std::string::npos);
  }
  std::cerr << " Done" << std::endl;
}

void testGithub852() {
  std::cout << " ----------------- Test Github852: Lines used to wedge bonds "
               "are too thick"
            << std::endl;
  {
    std::string smiles =
        "COc1cccc(NC(=O)[C@H](Cl)Sc2nc(ns2)c3ccccc3Cl)c1";  // made up
    std::string nameBase = "test852_1";
    RWMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    MolDraw2DUtils::prepareMolForDrawing(*m);

#ifdef RDK_CAIRO_BUILD
    {
      MolDraw2DCairo drawer(300, 300);
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      drawer.writeDrawingText(nameBase + ".png");
    }
#endif
    {
      std::ofstream outs((nameBase + ".svg").c_str());
      MolDraw2DSVG drawer(300, 300, outs);
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      outs.flush();
    }
    delete m;
  }
  {
    std::string smiles =
        "C[C@]12CC[C@@H]3c4ccc(cc4CC[C@H]3[C@@H]1CC[C@@H]2O)O";  // estradiol
    std::string nameBase = "test852_2";
    RWMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    MolDraw2DUtils::prepareMolForDrawing(*m);

#ifdef RDK_CAIRO_BUILD
    {
      MolDraw2DCairo drawer(300, 300);
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      drawer.writeDrawingText(nameBase + ".png");
    }
#endif
    {
      std::ofstream outs((nameBase + ".svg").c_str());
      MolDraw2DSVG drawer(300, 300, outs);
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      outs.flush();
    }
    delete m;
  }
  std::cerr << " Done" << std::endl;
}

void testGithub860() {
  std::cout << " ----------------- Test Github860: Atom symbols in wrong order "
               "if bond comes from right"
            << std::endl;
  {
    std::string smiles = "[15NH3+:1]-C#C-[15NH3+:2]";
    std::string nameBase = "test860_1";
    RWMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    MolDraw2DUtils::prepareMolForDrawing(*m);

#ifdef RDK_CAIRO_BUILD
    {
      MolDraw2DCairo drawer(300, 300);
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      drawer.writeDrawingText(nameBase + ".png");
    }
#endif
    {
      std::ofstream outs((nameBase + ".svg").c_str());
      MolDraw2DSVG drawer(300, 300, outs);
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      outs.flush();
    }
  }
  {
    std::string smiles =
        "[15NH3+:1]-C#C-C([15NH3+:2])([15NH3+:3])-C#C-[15NH3+:4]";
    std::string nameBase = "test860_2";
    RWMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    MolDraw2DUtils::prepareMolForDrawing(*m);

#ifdef RDK_CAIRO_BUILD
    {
      MolDraw2DCairo drawer(300, 300);
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      drawer.writeDrawingText(nameBase + ".png");
    }
#endif
    {
      std::ofstream outs((nameBase + ".svg").c_str());
      MolDraw2DSVG drawer(300, 300, outs);
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      outs.flush();
    }
  }
  {
    std::string smiles = "[15NH3+:1]-CCCCCCCC-[15NH3+:4]";
    std::string nameBase = "test860_3";
    RWMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    MolDraw2DUtils::prepareMolForDrawing(*m);

#ifdef RDK_CAIRO_BUILD
    {
      MolDraw2DCairo drawer(300, 300);
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      drawer.writeDrawingText(nameBase + ".png");
    }
#endif
    {
      std::ofstream outs((nameBase + ".svg").c_str());
      MolDraw2DSVG drawer(300, 300, outs);
      drawer.drawMolecule(*m);
      drawer.finishDrawing();
      outs.flush();
    }
  }
  std::cerr << " Done" << std::endl;
}

void testGithub910() {
  std::cout << " ----------------- Test Github #910: ugly coordinates "
               "generated for peptide chains"
            << std::endl;
  // this really isn't much of a test, but it does help visually confirm that
  // things are actually ok
  {
    // this is a ChEMBL molecule
    std::string smiles =
        "CSCC[C@H](NC(=O)[C@@H](CCC(N)=O)NC(=O)[C@@H](N)Cc1c[nH]c2ccccc12)C(="
        "O)"
        "NCC(=O)N[C@@H](Cc1c[nH]cn1)C(=O)N[C@@H](CO)C(=O)O";
    ROMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    RDDepict::compute2DCoords(*m);
    WedgeMolBonds(*m, &(m->getConformer()));
    std::ofstream outs("test910_1.svg");
    MolDraw2DSVG drawer(600, 300, outs);
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    outs.flush();
    delete m;
  }
  {  // now with Hs
    // this is a ChEMBL molecule
    std::string smiles =
        "CSCC[C@H](NC(=O)[C@@H](CCC(N)=O)NC(=O)[C@@H](N)Cc1c[nH]c2ccccc12)C(="
        "O)"
        "NCC(=O)N[C@@H](Cc1c[nH]cn1)C(=O)N[C@@H](CO)C(=O)O";
    RWMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    MolDraw2DUtils::prepareMolForDrawing(*m);
    std::ofstream outs("test910_2.svg");
    MolDraw2DSVG drawer(600, 300, outs);
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    outs.flush();
    delete m;
  }
  std::cerr << " Done" << std::endl;
}

void testGithub932() {
  std::cout << " ----------------- Test Github #932: mistake in SVG for "
               "wedged bonds"
            << std::endl;
  {
    std::string smiles = "CC[C@](F)(Cl)Br";
    RWMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    MolDraw2DUtils::prepareMolForDrawing(*m);
    MolDraw2DSVG drawer(200, 200);
    drawer.drawMolecule(*m);
    drawer.finishDrawing();

    std::string text = drawer.getDrawingText();
    TEST_ASSERT(text.find("evenoddstroke") == std::string::npos);
    delete m;
  }
  std::cerr << " Done" << std::endl;
}

void testGithub953() {
  std::cout << " ----------------- Test Github #953: default color should "
               "not be cyan"
            << std::endl;
  {
    std::string smiles = "[Nb]";
    RWMol *m = SmilesToMol(smiles);
    TEST_ASSERT(m);
    MolDraw2DUtils::prepareMolForDrawing(*m);
    MolDraw2DSVG drawer(200, 200);
    drawer.drawMolecule(*m);
    drawer.finishDrawing();

    std::string text = drawer.getDrawingText();
    TEST_ASSERT(text.find("#00FFFF") == std::string::npos);
    delete m;
  }
  std::cerr << " Done" << std::endl;
}

void testGithub983() {
  std::cout << " ----------------- Test Github #983: wedged bonds between "
               "chiral centers drawn improperly"
            << std::endl;
  {
    // this has an ugly drawing (wedged bond between chiral centers) but we
    // force it to be drawn that way just to check.
    std::string mb =
        "\n\
  Mrv1561 07241608122D\n\
\n\
  6  5  0  0  0  0            999 V2000\n\
    8.6830   -9.5982    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0\n\
    9.3975   -9.1857    0.0000 C   0  0  2  0  0  0  0  0  0  0  0  0\n\
   10.1120   -9.5982    0.0000 C   0  0  1  0  0  0  0  0  0  0  0  0\n\
    9.3975   -8.3607    0.0000 Cl  0  0  0  0  0  0  0  0  0  0  0  0\n\
   10.8264   -9.1857    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0\n\
   10.1120  -10.4232    0.0000 F   0  0  0  0  0  0  0  0  0  0  0  0\n\
  1  2  1  0  0  0  0\n\
  3  5  1  0  0  0  0\n\
  3  2  1  1  0  0  0\n\
  2  4  1  1  0  0  0\n\
  3  6  1  0  0  0  0\n\
M  END";
    RWMol *m = MolBlockToMol(mb, false, false);
    TEST_ASSERT(m);
    MolOps::sanitizeMol(*m);
    MolDraw2DUtils::prepareMolForDrawing(*m);

    MolDraw2DSVG drawer(200, 200);
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    std::string text = drawer.getDrawingText();
    std::ofstream outs("test983_1.svg");
    outs << text;
    outs.flush();
    TEST_ASSERT(text.find("<svg:path d='M 130.309,117.496 73.5169,75.8928 "
                          "65.8827,89.1161 130.309,117.496' "
                          "style='fill:#000000") != std::string::npos);
    delete m;
  }
  {
    std::string mb =
        "\n\
  Mrv1561 07241616282D\n\
\n\
 12 12  0  0  1  0            999 V2000\n\
   10.4656   -7.9623    0.0000 F   0  0  0  0  0  0  0  0  0  0  0  0\n\
    9.7496   -8.3748    0.0000 C   0  0  1  0  0  0  0  0  0  0  0  0\n\
    8.9075   -9.4746    0.0000 C   0  0  2  0  0  0  0  0  0  0  0  0\n\
    7.5671   -9.4746    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0\n\
    8.2373   -8.9934    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0\n\
    8.6497  -10.2651    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0\n\
    9.0392   -7.9623    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0\n\
    7.8249  -10.2651    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0\n\
    7.1547  -10.1792    0.0000 F   0  0  0  0  0  0  0  0  0  0  0  0\n\
    6.8567   -9.0622    0.0000 F   0  0  0  0  0  0  0  0  0  0  0  0\n\
   10.3338   -8.9591    0.0000 O   0  0  0  0  0  0  0  0  0  0  0  0\n\
    8.6841   -8.6669    0.0000 H   0  0  0  0  0  0  0  0  0  0  0  0\n\
  2  1  1  0  0  0  0\n\
  3  2  1  0  0  0  0\n\
  4  5  1  0  0  0  0\n\
  5  3  1  0  0  0  0\n\
  6  3  1  0  0  0  0\n\
  7  2  1  0  0  0  0\n\
  8  6  1  0  0  0  0\n\
  9  4  1  0  0  0  0\n\
 10  4  1  0  0  0  0\n\
  2 11  1  6  0  0  0\n\
  3 12  1  6  0  0  0\n\
  8  4  1  0  0  0  0\n\
M  END";
    RWMol *m = MolBlockToMol(mb);
    TEST_ASSERT(m);
    MolDraw2DUtils::prepareMolForDrawing(*m);
    TEST_ASSERT(m->getBondBetweenAtoms(2, 1)->getBondType() == Bond::SINGLE);
    TEST_ASSERT(m->getBondBetweenAtoms(2, 1)->getBondDir() == Bond::NONE);
    TEST_ASSERT(m->getBondBetweenAtoms(2, 4)->getBondType() == Bond::SINGLE);
    TEST_ASSERT(m->getBondBetweenAtoms(2, 4)->getBondDir() == Bond::BEGINWEDGE);

    MolDraw2DSVG drawer(200, 200);
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    std::string text = drawer.getDrawingText();
    std::ofstream outs("test983_2.svg");
    outs << text;
    outs.flush();
    TEST_ASSERT(text.find("<svg:path d='M 107.911,115.963 80.5887,91.4454 "
                          "75.9452,97.9126 107.911,115.963' "
                          "style='fill:#000000;") != std::string::npos);

    MolDraw2DUtils::prepareMolForDrawing(*m);
    TEST_ASSERT(m->getBondBetweenAtoms(2, 1)->getBondType() == Bond::SINGLE);
    TEST_ASSERT(m->getBondBetweenAtoms(2, 1)->getBondDir() == Bond::NONE);
    TEST_ASSERT(m->getBondBetweenAtoms(2, 4)->getBondType() == Bond::SINGLE);
    TEST_ASSERT(m->getBondBetweenAtoms(2, 4)->getBondDir() == Bond::BEGINWEDGE);

    RWMol nm(*m);
    MolDraw2DUtils::prepareMolForDrawing(nm);
    TEST_ASSERT(nm.getBondBetweenAtoms(2, 1)->getBondType() == Bond::SINGLE);
    TEST_ASSERT(nm.getBondBetweenAtoms(2, 1)->getBondDir() == Bond::NONE);
    TEST_ASSERT(nm.getBondBetweenAtoms(2, 4)->getBondType() == Bond::SINGLE);
    TEST_ASSERT(nm.getBondBetweenAtoms(2, 4)->getBondDir() == Bond::BEGINWEDGE);

    delete m;
  }

  std::cerr << " Done" << std::endl;
}

void testDeuteriumTritium() {
  std::cout << " ----------------- Test Deuterium, Tritium" << std::endl;
  {
    std::string deuterium = "C([2H])([2H])([2H])[2H]";
    ROMol *m = SmilesToMol(deuterium);
    RDDepict::compute2DCoords(*m);
    std::string nameBase = "testNoDeuterium";
    std::ofstream outs((nameBase + ".svg").c_str());
    MolDraw2DSVG drawer(300, 300, outs);
    drawer.drawOptions().atomLabelDeuteriumTritium = false;
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    outs.close();
    std::ifstream ins((nameBase + ".svg").c_str());
    bool ok = true;
    unsigned int count = 0;
    while (ok) {
      std::string line;
      std::getline(ins, line);
      ok = (ins.good() && !ins.eof());
      if (!ok) continue;
      if ((line.find("baseline-shift:super") != std::string::npos)
        && (line.find(">2<") != std::string::npos)
        && (line.find(">H<") != std::string::npos))
        ++count;
    }
    TEST_ASSERT(count == 4);
    delete m;
  }
  {
    std::string tritium = "C([3H])([3H])([3H])[3H]";
    ROMol *m = SmilesToMol(tritium);
    RDDepict::compute2DCoords(*m);
    std::string nameBase = "testNoTritium";
    std::ofstream outs((nameBase + ".svg").c_str());
    MolDraw2DSVG drawer(300, 300, outs);
    drawer.drawOptions().atomLabelDeuteriumTritium = false;
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    outs.close();
    std::ifstream ins((nameBase + ".svg").c_str());
    bool ok = true;
    unsigned int count = 0;
    while (ok) {
      std::string line;
      std::getline(ins, line);
      ok = (ins.good() && !ins.eof());
      if (!ok) continue;
      if ((line.find("baseline-shift:super") != std::string::npos)
        && (line.find(">3<") != std::string::npos)
        && (line.find(">H<") != std::string::npos))
        ++count;
    }
    TEST_ASSERT(count == 4);
    delete m;
  }
  {
    std::string deuterium = "C([2H])([2H])([2H])[2H]";
    ROMol *m = SmilesToMol(deuterium);
    RDDepict::compute2DCoords(*m);
    std::string nameBase = "testDeuterium";
    std::ofstream outs((nameBase + ".svg").c_str());
    MolDraw2DSVG drawer(300, 300, outs);
    drawer.drawOptions().atomLabelDeuteriumTritium = true;
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    outs.close();
    std::ifstream ins((nameBase + ".svg").c_str());
    bool ok = true;
    unsigned int count = 0;
    while (ok) {
      std::string line;
      std::getline(ins, line);
      ok = (ins.good() && !ins.eof());
      if (!ok) continue;
      if ((line.find("baseline-shift:super") == std::string::npos)
        && (line.find(">2<") == std::string::npos)
        && (line.find(">D<") != std::string::npos))
        ++count;
    }
    TEST_ASSERT(count == 4);
    delete m;
  }
  {
    std::string tritium = "C([3H])([3H])([3H])[3H]";
    ROMol *m = SmilesToMol(tritium);
    RDDepict::compute2DCoords(*m);
    std::string nameBase = "testTritium";
    std::ofstream outs((nameBase + ".svg").c_str());
    MolDraw2DSVG drawer(300, 300, outs);
    drawer.drawOptions().atomLabelDeuteriumTritium = true;
    drawer.drawMolecule(*m);
    drawer.finishDrawing();
    outs.close();
    std::ifstream ins((nameBase + ".svg").c_str());
    bool ok = true;
    unsigned int count = 0;
    while (ok) {
      std::string line;
      std::getline(ins, line);
      ok = (ins.good() && !ins.eof());
      if (!ok) continue;
      if ((line.find("baseline-shift:super") == std::string::npos)
        && (line.find(">3<") == std::string::npos)
        && (line.find(">T<") != std::string::npos))
        ++count;
    }
    TEST_ASSERT(count == 4);
    delete m;
  }
  std::cerr << " Done" << std::endl;
}

int main() {
  RDLog::InitLogs();
#if 1
  test1();
  test2();
  test4();
  test5();
  test6();
  test7();
  test8PrepareMolForDrawing();
  testMultiThreaded();
  testGithub781();
  test3();
  testGithub774();
  test9MolLegends();
  testGithub852();
  testGithub860();
#endif
  testGithub910();
  testGithub932();
  testGithub953();
  testGithub983();
  testDeuteriumTritium();
}
