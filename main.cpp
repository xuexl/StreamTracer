#include<vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStreamTracer.h>

#include<vtkPoints.h>
#include<vtkLine.h>
#include<vtkDoubleArray.h>
#include<vtkPointData.h>
#include<vtkWarpVector.h>
#include<vtkPointSource.h>
#include<vtkDelaunay3D.h>
#include<vtkArrayCalculator.h>
#include<vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkLookupTable.h>
#include<vtkRibbonFilter.h>


#include<QString>
#include<string>
#include<iostream>


namespace  {
const char* lorentzFunc(int a, int b, double c){
    
    QString qs("iHat*%1*(coordsY-coordsX) + jHat*(coordsX*(%2-coordsZ)-coordsY) + kHat*(coordsX*coordsY-%3*coordsZ)");
    std::cout<<qs.arg(a).arg(b).arg(c).toStdString().c_str()<<std::endl;
//    return qs.arg(a).arg(b).arg(c).toStdString().data();   
//    return "iHat*20*(coordsY-coordsX) + jHat*(coordsX*(18-coordsZ)-coordsY) + kHat*(coordsX*coordsY-2.66667*coordsZ)";
    return "(iHat*coordsX+jHat*coordsY+kHat*coordsZ)";
//    return "iHat";
}
    
}

int main()
{

    auto points = vtkSmartPointer<vtkPointSource>::New();
    points->SetNumberOfPoints(100);
    points->SetDistributionToUniform();
    points->SetCenter(0,0,0);
    points->SetRadius(10);
    
    auto tets = vtkSmartPointer<vtkDelaunay3D>::New();
    tets->SetInputConnection(points->GetOutputPort());
    
    auto calc = vtkSmartPointer<vtkArrayCalculator>::New();
    calc->SetInputConnection(tets->GetOutputPort());    
    calc->SetAttributeTypeToPointData();
    calc->AddCoordinateScalarVariable("coordsX",0);
    calc->AddCoordinateScalarVariable("coordsY",1);
    calc->AddCoordinateScalarVariable("coordsZ",2);
    calc->SetFunction(lorentzFunc(20,18, 8/3.0));
    calc->SetResultArrayName("vectors");
        
//    double vectorsRange[2] = {0, 100};
//    calc->GetDataSetOutput()->GetPointData()->GetArray("vectors")->GetRange(vectorsRange);


    auto smap = vtkSmartPointer<vtkPolyDataMapper>::New();
    smap->SetInputConnection(calc->GetOutputPort());
//    smap->SelectColorArray("vectors");
//    smap->SetScalarRange(vectorsRange);
    
    auto sactor = vtkSmartPointer<vtkActor>::New();
    sactor->SetMapper(smap);
    
    
    //和图形显示有关
    auto seed = vtkSmartPointer<vtkPointSource>::New();
    seed->SetRadius(0.1);   //让图形更分散
    //Center和图形原点保持一致
    seed->SetCenter(.0,.0,.0);  //这里设置对图形的影响    ?????
    seed->SetNumberOfPoints(10);    //每个点对应一条线
    
    auto streamers = vtkSmartPointer<vtkStreamTracer>::New();
    streamers->SetInputConnection(calc->GetOutputPort());
    streamers->SetSourceConnection(seed->GetOutputPort());      //每个点都会计算一条流线
//    streamers->SetStartPosition(.0,.0,.0);          //设置起始点, 也可以通过指定单元id，单元子id和参数坐标来指定
    
    //Specify the maximum length of a streamline expressed in LENGTH_UNIT 
    //控制流线的最大长度（度量单位由MaximumPropagationUnit变量指定）
    streamers->SetMaximumPropagation(1000);  
    
    streamers->SetMinimumIntegrationStep(0.1);  //Specify the Minimum step size used for line integration
    streamers->SetMaximumIntegrationStep(1.0);  //Specify the Maximum step size used for line integration
    streamers->SetInitialIntegrationStep(0.1);  //Specify the Initial step size used for line integration
    streamers->SetIntegratorType(2);    //积分类型
    streamers->SetIntegrationDirectionToBoth();  //积分方向
    streamers->Update();
    
    
        auto magCalc = vtkSmartPointer<vtkArrayCalculator>::New();
      magCalc->SetInputConnection(streamers->GetOutputPort());
      magCalc->AddVectorArrayName("vectors");
      magCalc->SetResultArrayName("MagVelocity");
      magCalc->SetFunction("mag(vectors)");
    
      magCalc->Update();
      double magVelocityRange[2];
      magCalc->GetDataSetOutput()->GetPointData()->GetArray("MagVelocity")->GetRange(magVelocityRange);
      
      auto ribbonFilter = vtkSmartPointer<vtkRibbonFilter>::New();
        ribbonFilter->SetInputConnection(magCalc->GetOutputPort());
        ribbonFilter->SetWidth(0.005);
    
    
    auto mapStreamers = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapStreamers->SetInputConnection(magCalc->GetOutputPort());
    mapStreamers->SelectColorArray("MagVelocity");
     mapStreamers->SetScalarRange(magVelocityRange);
    
//    mapStreamers->SetScalarRange(lineSource->GetOutput()->GetPointData()->GetScalars()->GetRange());
    auto streamersActor = vtkSmartPointer<vtkActor>::New();
    streamersActor->SetMapper(mapStreamers);
    
    
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    
    auto interactor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(renderWindow);
    
   
    //多个视口
    double leftViewport[4] = {0.0, 0.0, 0.5, 1.0};
     double rightViewport[4] = {0.5, 0.0, 1.0, 1.0};
    
    {
        auto renderer = vtkSmartPointer<vtkRenderer>::New();    
        renderer->AddActor(sactor);
        renderer->SetViewport(leftViewport);
        renderer->SetBackground(.1, .6, .1); 
        renderer->ResetCamera();
        renderWindow->AddRenderer(renderer);                
    }
    
     {
         auto renderer = vtkSmartPointer<vtkRenderer>::New();    
         renderer->AddActor(streamersActor);
         renderer->SetViewport(rightViewport);
         renderer->SetBackground(.5, .6, .4); 
         renderer->ResetCamera();
         renderWindow->AddRenderer(renderer);                
     }
    
    

    renderWindow->SetSize(1040, 800);
    renderWindow->Render();
    interactor->Start();
    
    
    return 1;
}
