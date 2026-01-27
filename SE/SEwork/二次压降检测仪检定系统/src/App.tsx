import { useState } from 'react';
import { DeviceInfoInput } from './components/DeviceInfoInput';
import { ExperimentDataInput } from './components/ExperimentDataInput';
import { ResultCapture } from './components/ResultCapture';
import { ReportView } from './components/ReportView';
import { DataQuery } from './components/DataQuery';
import { ClipboardList, FlaskConical, Camera, FileText, Search } from 'lucide-react';

type TabType = 'device' | 'experiment' | 'capture' | 'report' | 'query';

interface Detection {
  id: string;
  deviceInfo: any;
  experimentData: any;
  resultData: any;
  timestamp: number;
}

export default function App() {
  const [activeTab, setActiveTab] = useState<TabType>('device');
  const [currentDetection, setCurrentDetection] = useState<Detection | null>(null);
  const [detections, setDetections] = useState<Detection[]>(() => {
    const saved = localStorage.getItem('detections');
    return saved ? JSON.parse(saved) : [];
  });

  const saveDetection = (data: Partial<Detection>) => {
    const updated = {
      ...currentDetection,
      ...data,
      id: currentDetection?.id || Date.now().toString(),
      timestamp: Date.now()
    } as Detection;
    
    setCurrentDetection(updated);
    
    const existingIndex = detections.findIndex(d => d.id === updated.id);
    let newDetections;
    if (existingIndex >= 0) {
      newDetections = [...detections];
      newDetections[existingIndex] = updated;
    } else {
      newDetections = [...detections, updated];
    }
    
    setDetections(newDetections);
    localStorage.setItem('detections', JSON.stringify(newDetections));
  };

  const tabs = [
    { id: 'device' as TabType, label: '设备信息', icon: ClipboardList },
    { id: 'experiment' as TabType, label: '实验数据', icon: FlaskConical },
    { id: 'capture' as TabType, label: '结果采集', icon: Camera },
    { id: 'report' as TabType, label: '检测报告', icon: FileText },
    { id: 'query' as TabType, label: '数据查询', icon: Search },
  ];

  return (
    <div className="min-h-screen bg-gradient-to-br from-blue-50 to-slate-100">
      {/* Header */}
      <header className="bg-white shadow-sm border-b border-slate-200">
        <div className="max-w-7xl mx-auto px-6 py-4">
          <h1 className="text-blue-600">互感器二次压降检测仪检定系统</h1>
          <p className="text-slate-600 mt-1">Transformer Secondary Voltage Drop Detector Verification System</p>
        </div>
      </header>

      {/* Navigation */}
      <nav className="bg-white border-b border-slate-200 shadow-sm">
        <div className="max-w-7xl mx-auto px-6">
          <div className="flex gap-2">
            {tabs.map(tab => {
              const Icon = tab.icon;
              return (
                <button
                  key={tab.id}
                  onClick={() => setActiveTab(tab.id)}
                  className={`flex items-center gap-2 px-6 py-4 border-b-2 transition-colors ${
                    activeTab === tab.id
                      ? 'border-blue-500 text-blue-600 bg-blue-50'
                      : 'border-transparent text-slate-600 hover:text-slate-900 hover:bg-slate-50'
                  }`}
                >
                  <Icon className="w-5 h-5" />
                  <span>{tab.label}</span>
                </button>
              );
            })}
          </div>
        </div>
      </nav>

      {/* Main Content */}
      <main className="max-w-7xl mx-auto px-6 py-8">
        {activeTab === 'device' && (
          <DeviceInfoInput
            data={currentDetection?.deviceInfo}
            onSave={(deviceInfo) => {
              saveDetection({ deviceInfo });
              setActiveTab('experiment');
            }}
          />
        )}
        
        {activeTab === 'experiment' && (
          <ExperimentDataInput
            data={currentDetection?.experimentData}
            onSave={(experimentData) => {
              saveDetection({ experimentData });
              setActiveTab('capture');
            }}
            onBack={() => setActiveTab('device')}
          />
        )}
        
        {activeTab === 'capture' && (
          <ResultCapture
            onCapture={(resultData) => {
              saveDetection({ resultData });
              setActiveTab('report');
            }}
            onBack={() => setActiveTab('experiment')}
          />
        )}
        
        {activeTab === 'report' && (
          <ReportView
            detection={currentDetection}
            onBack={() => setActiveTab('capture')}
            onNew={() => {
              setCurrentDetection(null);
              setActiveTab('device');
            }}
          />
        )}
        
        {activeTab === 'query' && (
          <DataQuery detections={detections} onView={(detection) => {
            setCurrentDetection(detection);
            setActiveTab('report');
          }} />
        )}
      </main>
    </div>
  );
}
