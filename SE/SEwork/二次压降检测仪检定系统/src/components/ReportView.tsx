import { ArrowLeft, Printer, FileDown, Plus } from 'lucide-react';

interface Detection {
  id: string;
  deviceInfo: any;
  experimentData: any;
  resultData: any;
  timestamp: number;
}

interface ReportViewProps {
  detection: Detection | null;
  onBack: () => void;
  onNew: () => void;
}

export function ReportView({ detection, onBack, onNew }: ReportViewProps) {
  if (!detection?.deviceInfo) {
    return (
      <div className="bg-white rounded-lg shadow-md p-8 text-center">
        <p className="text-slate-600 mb-4">暂无检测数据，请先完成设备信息录入和实验数据采集</p>
        <button
          onClick={onNew}
          className="px-6 py-3 bg-blue-600 text-white rounded-lg hover:bg-blue-700 transition"
        >
          开始新检测
        </button>
      </div>
    );
  }

  const handlePrint = () => {
    window.print();
  };

  const handleExport = () => {
    const reportData = {
      ...detection,
      exportDate: new Date().toISOString()
    };
    const blob = new Blob([JSON.stringify(reportData, null, 2)], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `检测报告_${detection.deviceInfo.productCode}_${new Date().toISOString().split('T')[0]}.json`;
    a.click();
    URL.revokeObjectURL(url);
  };

  const { deviceInfo, experimentData, resultData } = detection;

  return (
    <div className="space-y-6">
      {/* Action Buttons */}
      <div className="flex justify-between items-center print:hidden">
        <button
          onClick={onBack}
          className="flex items-center gap-2 px-4 py-2 text-slate-700 border-2 border-slate-300 rounded-lg hover:bg-slate-50 transition"
        >
          <ArrowLeft className="w-5 h-5" />
          返回
        </button>
        <div className="flex gap-3">
          <button
            onClick={handleExport}
            className="flex items-center gap-2 px-4 py-2 text-blue-600 border-2 border-blue-600 rounded-lg hover:bg-blue-50 transition"
          >
            <FileDown className="w-5 h-5" />
            导出数据
          </button>
          <button
            onClick={handlePrint}
            className="flex items-center gap-2 px-4 py-2 bg-green-600 text-white rounded-lg hover:bg-green-700 transition"
          >
            <Printer className="w-5 h-5" />
            打印报告
          </button>
          <button
            onClick={onNew}
            className="flex items-center gap-2 px-4 py-2 bg-blue-600 text-white rounded-lg hover:bg-blue-700 transition"
          >
            <Plus className="w-5 h-5" />
            新建检测
          </button>
        </div>
      </div>

      {/* Report Content */}
      <div className="bg-white rounded-lg shadow-md p-8 print:shadow-none">
        {/* Header */}
        <div className="text-center border-b-2 border-slate-300 pb-6 mb-6">
          <h1 className="text-blue-600 mb-2">互感器二次压降检测仪检定报告</h1>
          <p className="text-slate-600">Transformer Secondary Voltage Drop Detector Verification Report</p>
          <p className="text-slate-500 mt-2">报告编号: {detection.id}</p>
        </div>

        {/* Device Information */}
        <section className="mb-8">
          <h2 className="text-slate-900 mb-4 pb-2 border-b border-slate-200">
            一、被检设备信息
          </h2>
          <div className="grid grid-cols-2 gap-4">
            <div className="flex">
              <span className="text-slate-600 w-32">产品编号：</span>
              <span>{deviceInfo.productCode}</span>
            </div>
            <div className="flex">
              <span className="text-slate-600 w-32">产品名称：</span>
              <span>{deviceInfo.productName}</span>
            </div>
            <div className="flex">
              <span className="text-slate-600 w-32">制造商：</span>
              <span>{deviceInfo.manufacturer}</span>
            </div>
            <div className="flex">
              <span className="text-slate-600 w-32">产品型号：</span>
              <span>{deviceInfo.modelNumber || '未填写'}</span>
            </div>
            <div className="flex">
              <span className="text-slate-600 w-32">生产日期：</span>
              <span>{deviceInfo.productionDate}</span>
            </div>
            <div className="flex">
              <span className="text-slate-600 w-32">送检日期：</span>
              <span>{deviceInfo.inspectionDate}</span>
            </div>
            <div className="flex">
              <span className="text-slate-600 w-32">产地：</span>
              <span>{deviceInfo.origin || '未填写'}</span>
            </div>
            <div className="flex">
              <span className="text-slate-600 w-32">备注：</span>
              <span>{deviceInfo.notes || '无'}</span>
            </div>
          </div>
        </section>

        {/* Experiment Data */}
        <section className="mb-8">
          <h2 className="text-slate-900 mb-4 pb-2 border-b border-slate-200">
            二、检测实验数据
          </h2>
          {experimentData?.rows && (
            <div className="overflow-x-auto">
              <table className="w-full border-collapse">
                <thead>
                  <tr className="bg-blue-600 text-white">
                    <th className="border border-blue-700 px-4 py-2">项目</th>
                    <th className="border border-blue-700 px-4 py-2">档位</th>
                    <th className="border border-blue-700 px-4 py-2">百分比</th>
                    <th className="border border-blue-700 px-4 py-2">数据下限</th>
                    <th className="border border-blue-700 px-4 py-2">数据上限</th>
                    <th className="border border-blue-700 px-4 py-2">实测数据</th>
                  </tr>
                </thead>
                <tbody>
                  {experimentData.rows.map((row: any, index: number) => (
                    <tr key={row.id} className={index % 2 === 0 ? 'bg-slate-50' : ''}>
                      <td className="border border-slate-300 px-4 py-2">{row.item}</td>
                      <td className="border border-slate-300 px-4 py-2">{row.gear}</td>
                      <td className="border border-slate-300 px-4 py-2">{row.percentage}</td>
                      <td className="border border-slate-300 px-4 py-2">{row.lowerLimit}</td>
                      <td className="border border-slate-300 px-4 py-2">{row.upperLimit}</td>
                      <td className="border border-slate-300 px-4 py-2 bg-blue-50">
                        {row.measuredValue || '-'}
                      </td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          )}
        </section>

        {/* Test Results */}
        {resultData && (
          <section className="mb-8">
            <h2 className="text-slate-900 mb-4 pb-2 border-b border-slate-200">
              三、检测结果输出
            </h2>
            
            {/* Basic Results */}
            <div className="grid grid-cols-3 gap-4 mb-6">
              <div className="bg-slate-50 p-4 rounded">
                <span className="text-slate-600">二次电压：</span>
                <span>{resultData.secondaryVoltage}</span>
              </div>
              <div className="bg-slate-50 p-4 rounded">
                <span className="text-slate-600">tanφ：</span>
                <span>{resultData.tanPhi}</span>
              </div>
              <div className="bg-slate-50 p-4 rounded">
                <span className="text-slate-600">计量点编号：</span>
                <span>{resultData.meteringPoint}</span>
              </div>
              <div className="bg-slate-50 p-4 rounded">
                <span className="text-slate-600">温度：</span>
                <span>{resultData.temperature}°C</span>
              </div>
              <div className="bg-slate-50 p-4 rounded">
                <span className="text-slate-600">测试日期：</span>
                <span>{resultData.testDate}</span>
              </div>
              <div className="bg-slate-50 p-4 rounded">
                <span className="text-slate-600">湿度：</span>
                <span>{resultData.humidity}%</span>
              </div>
            </div>

            {/* Detailed Measurements */}
            <div className="overflow-x-auto">
              <table className="w-full border-collapse">
                <thead>
                  <tr className="bg-slate-700 text-white">
                    <th className="border border-slate-600 px-4 py-2">相位</th>
                    <th className="border border-slate-600 px-4 py-2">f(%)</th>
                    <th className="border border-slate-600 px-4 py-2">d(分)</th>
                    <th className="border border-slate-600 px-4 py-2">dU(%)</th>
                    <th className="border border-slate-600 px-4 py-2">Upt:U</th>
                    <th className="border border-slate-600 px-4 py-2">Uyb:U</th>
                  </tr>
                </thead>
                <tbody>
                  {['ao', 'bo', 'co'].map((phase, index) => (
                    <tr key={phase} className={index % 2 === 0 ? 'bg-slate-50' : ''}>
                      <td className="border border-slate-300 px-4 py-2">{phase}</td>
                      <td className="border border-slate-300 px-4 py-2">
                        {resultData.measurements[phase].f}
                      </td>
                      <td className="border border-slate-300 px-4 py-2">
                        {resultData.measurements[phase].d}
                      </td>
                      <td className="border border-slate-300 px-4 py-2">
                        {resultData.measurements[phase].dU}
                      </td>
                      <td className="border border-slate-300 px-4 py-2">
                        {resultData.measurements[phase].Upt}
                      </td>
                      <td className="border border-slate-300 px-4 py-2">
                        {resultData.measurements[phase].Uyb}
                      </td>
                    </tr>
                  ))}
                </tbody>
              </table>
              <div className="mt-2 text-right text-slate-600">
                PT侧不带自校 / 按1测量 按2储存 / r% {resultData.rValue}
              </div>
            </div>
          </section>
        )}

        {/* Footer */}
        <section className="mt-12 pt-6 border-t-2 border-slate-300">
          <div className="grid grid-cols-2 gap-8">
            <div>
              <p className="text-slate-600">检测员签名：_______________</p>
            </div>
            <div>
              <p className="text-slate-600">审核员签名：_______________</p>
            </div>
            <div>
              <p className="text-slate-600">检测日期：{new Date(detection.timestamp).toLocaleDateString('zh-CN')}</p>
            </div>
            <div>
              <p className="text-slate-600">报告日期：{new Date().toLocaleDateString('zh-CN')}</p>
            </div>
          </div>
        </section>
      </div>
    </div>
  );
}
