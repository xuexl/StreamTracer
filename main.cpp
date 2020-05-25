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



int main(int argc, char *argv[])
{

    vtkSmartPointer<vtkPoints> points =
        vtkSmartPointer<vtkPoints>::New();
      points->InsertNextPoint(0.0, 0.0, 0.0);
      points->InsertNextPoint(1.0, 0.0, 0.0);
      points->InsertNextPoint(2.0, 0.0, 0.0);
      points->InsertNextPoint(3.0, 0.0, 0.0);
      points->InsertNextPoint(4.0, 0.0, 0.0);
    
      vtkSmartPointer<vtkCellArray> lines =
        vtkSmartPointer<vtkCellArray>::New();
      vtkSmartPointer<vtkLine> line =
        vtkSmartPointer<vtkLine>::New();
      line->GetPointIds()->SetId(0, 0);
      line->GetPointIds()->SetId(1, 1);
      lines->InsertNextCell(line);
      line->GetPointIds()->SetId(0, 1);
      line->GetPointIds()->SetId(1, 2);
      lines->InsertNextCell(line);
      line->GetPointIds()->SetId(0, 2);
      line->GetPointIds()->SetId(1, 3);
      lines->InsertNextCell(line);
      line->GetPointIds()->SetId(0, 3);
      line->GetPointIds()->SetId(1, 4);
      lines->InsertNextCell(line);
    
      vtkSmartPointer<vtkDoubleArray> warpData =
        vtkSmartPointer<vtkDoubleArray>::New();
      warpData->SetNumberOfComponents(3);
      warpData->SetName("warpData");
      double warp[3] = {0.0, 0.0, 0.0};
      warp[1] = 0.0;
      warpData->InsertNextTuple(warp);
      warp[1] = 0.1;
      warpData->InsertNextTuple(warp);
      warp[1] = 0.3;
      warpData->InsertNextTuple(warp);
      warp[1] = 0.0;
      warpData->InsertNextTuple(warp);
      warp[1] = 0.1;
      warpData->InsertNextTuple(warp);
    
      vtkSmartPointer<vtkPolyData> polydata =
        vtkSmartPointer<vtkPolyData>::New();
      polydata->SetPoints(points);
      polydata->SetLines(lines);
      polydata->GetPointData()->AddArray(warpData);
      polydata->GetPointData()->SetActiveVectors(warpData->GetName());
      
      vtkSmartPointer<vtkWarpVector> warpVector =
        vtkSmartPointer<vtkWarpVector>::New();
      warpVector->SetInputData(polydata);
      warpVector->Update();
          

    auto smap = vtkSmartPointer<vtkPolyDataMapper>::New();
    smap->SetInputConnection(warpVector->GetOutputPort());
    auto sactor = vtkSmartPointer<vtkActor>::New();
    sactor->SetMapper(smap);
    
    auto seed = vtkSmartPointer<vtkPointSource>::New();
    seed->SetRadius(0.1);
    seed->SetCenter(.0,.0,.0);
    seed->SetNumberOfPoints(8000);
    
    auto streamers = vtkSmartPointer<vtkStreamTracer>::New();
    streamers->SetInputConnection(warpVector->GetOutputPort());
    streamers->SetSourceConnection(seed->GetOutputPort());      //每个点都会计算一条流线
    streamers->SetStartPosition(.0,.0,.0);          //设置起始点, 也可以通过指定单元id，单元子id和参数坐标来指定
    
    //Specify the maximum length of a streamline expressed in LENGTH_UNIT 
    //控制流线的最大长度（度量单位由MaximumPropagationUnit变量指定）
    streamers->SetMaximumPropagation(10);  
    
    streamers->SetMinimumIntegrationStep(0.1);  //Specify the Minimum step size used for line integration
    streamers->SetMaximumIntegrationStep(1.0);  //Specify the Maximum step size used for line integration
    streamers->SetInitialIntegrationStep(0.1);  //Specify the Initial step size used for line integration
    streamers->SetIntegratorType(2);    //积分类型
    streamers->SetIntegrationDirectionToBoth();  //积分方向
    streamers->Update();
    
    auto mapStreamers = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapStreamers->SetInputConnection(streamers->GetOutputPort());
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
