import { useState } from 'react';
import { Camera, ArrowLeft, ArrowRight, Upload, RefreshCw } from 'lucide-react';

interface ResultData {
  secondaryVoltage: string;
  meteringPoint: string;
  testDate: string;
  tanPhi: string;
  temperature: string;
  humidity: string;
  measurements: {
    ao: { f: string; d: string; dU: string; Upt: string; Uyb: string };
    bo: { f: string; d: string; dU: string; Upt: string; Uyb: string };
    co: { f: string; d: string; dU: string; Upt: string; Uyb: string };
  };
  finalResults: string[];
  rValue: string;
}

interface ResultCaptureProps {
  onCapture: (data: ResultData) => void;
  onBack: () => void;
}

export function ResultCapture({ onCapture, onBack }: ResultCaptureProps) {
  const [isCapturing, setIsCapturing] = useState(false);
  const [capturedImage, setCapturedImage] = useState<string | null>(null);
  const [resultData, setResultData] = useState<ResultData>({
    secondaryVoltage: '14.8 U',
    meteringPoint: '86678765',
    testDate: '2023102',
    tanPhi: '-31.2336',
    temperature: '21.9',
    humidity: '34.6',
    measurements: {
      ao: { f: '0.0399', d: '-0.407', dU: '0.5268', Upt: '12.575', Uyb: '33.575' },
      bo: { f: '0.1332', d: '-0.886', dU: '0.5650', Upt: '26.550', Uyb: '81.269' },
      co: { f: '0.7345', d: '-0.408', dU: '0.5401', Upt: '51.328', Uyb: '90.226' }
    },
    finalResults: ['5.037', '6.734', '6266.0', '5.415'],
    rValue: '0.718'
  });

  const simulateCapture = () => {
    setIsCapturing(true);
    setTimeout(() => {
      setCapturedImage('captured');
      setIsCapturing(false);
    }, 1500);
  };

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    onCapture(resultData);
  };

  const updateMeasurement = (
    phase: 'ao' | 'bo' | 'co',
    field: keyof ResultData['measurements']['ao'],
    value: string
  ) => {
    setResultData(prev => ({
      ...prev,
      measurements: {
        ...prev.measurements,
        [phase]: {
          ...prev.measurements[phase],
          [field]: value
        }
      }
    }));
  };

  return (
    <div className="space-y-6">
      {/* Capture Section */}
      <div className="bg-white rounded-lg shadow-md p-8">
        <div className="mb-6">
          <h2 className="text-slate-900">检测结果采集</h2>
          <p className="text-slate-600 mt-1">从检测仪屏幕采集测试结果并进行OCR识别</p>
        </div>

        <div className="flex flex-col items-center gap-6">
          <div className="w-full max-w-2xl aspect-video bg-slate-100 rounded-lg border-2 border-dashed border-slate-300 flex items-center justify-center">
            {!capturedImage ? (
              <div className="text-center">
                <Camera className="w-16 h-16 text-slate-400 mx-auto mb-4" />
                <p className="text-slate-500">准备采集检测仪屏幕图像</p>
              </div>
            ) : isCapturing ? (
              <div className="text-center">
                <RefreshCw className="w-16 h-16 text-blue-600 mx-auto mb-4 animate-spin" />
                <p className="text-blue-600">正在采集并识别...</p>
              </div>
            ) : (
              <div className="text-center">
                <div className="bg-slate-800 text-slate-100 p-6 rounded-lg mx-4">
                  <p className="mb-2">检测结果输出</p>
                  <div className="grid grid-cols-2 gap-4 text-sm">
                    <div>二次电压: {resultData.secondaryVoltage}</div>
                    <div>tanφ: {resultData.tanPhi}</div>
                    <div>计量点编号: {resultData.meteringPoint}</div>
                    <div>温度: {resultData.temperature}°C</div>
                    <div>测试日期: {resultData.testDate}</div>
                    <div>湿度: {resultData.humidity}%</div>
                  </div>
                </div>
              </div>
            )}
          </div>

          <div className="flex gap-4">
            <button
              type="button"
              onClick={simulateCapture}
              disabled={isCapturing}
              className="flex items-center gap-2 px-6 py-3 bg-green-600 text-white rounded-lg hover:bg-green-700 disabled:bg-slate-400 transition shadow-md"
            >
              {isCapturing ? (
                <>
                  <RefreshCw className="w-5 h-5 animate-spin" />
                  采集中...
                </>
              ) : (
                <>
                  <Camera className="w-5 h-5" />
                  模拟图像采集
                </>
              )}
            </button>
            <button
              type="button"
              className="flex items-center gap-2 px-6 py-3 text-slate-700 border-2 border-slate-300 rounded-lg hover:bg-slate-50 transition"
            >
              <Upload className="w-5 h-5" />
              上传图像
            </button>
          </div>
        </div>
      </div>

      {/* Result Data Form */}
      {capturedImage && !isCapturing && (
        <form onSubmit={handleSubmit} className="bg-white rounded-lg shadow-md p-8">
          <div className="mb-6">
            <h2 className="text-slate-900">检测结果数据</h2>
            <p className="text-slate-600 mt-1">确认或修改OCR识别的数据</p>
          </div>

          <div className="space-y-6">
            {/* Basic Info */}
            <div className="grid grid-cols-2 md:grid-cols-3 gap-4">
              <div>
                <label className="block text-slate-700 mb-2">二次电压</label>
                <input
                  type="text"
                  value={resultData.secondaryVoltage}
                  onChange={(e) => setResultData({ ...resultData, secondaryVoltage: e.target.value })}
                  className="w-full px-3 py-2 border border-slate-300 rounded focus:ring-2 focus:ring-blue-500 outline-none"
                />
              </div>
              <div>
                <label className="block text-slate-700 mb-2">tanφ</label>
                <input
                  type="text"
                  value={resultData.tanPhi}
                  onChange={(e) => setResultData({ ...resultData, tanPhi: e.target.value })}
                  className="w-full px-3 py-2 border border-slate-300 rounded focus:ring-2 focus:ring-blue-500 outline-none"
                />
              </div>
              <div>
                <label className="block text-slate-700 mb-2">计量点编号</label>
                <input
                  type="text"
                  value={resultData.meteringPoint}
                  onChange={(e) => setResultData({ ...resultData, meteringPoint: e.target.value })}
                  className="w-full px-3 py-2 border border-slate-300 rounded focus:ring-2 focus:ring-blue-500 outline-none"
                />
              </div>
              <div>
                <label className="block text-slate-700 mb-2">温度 (°C)</label>
                <input
                  type="text"
                  value={resultData.temperature}
                  onChange={(e) => setResultData({ ...resultData, temperature: e.target.value })}
                  className="w-full px-3 py-2 border border-slate-300 rounded focus:ring-2 focus:ring-blue-500 outline-none"
                />
              </div>
              <div>
                <label className="block text-slate-700 mb-2">测试日期</label>
                <input
                  type="text"
                  value={resultData.testDate}
                  onChange={(e) => setResultData({ ...resultData, testDate: e.target.value })}
                  className="w-full px-3 py-2 border border-slate-300 rounded focus:ring-2 focus:ring-blue-500 outline-none"
                />
              </div>
              <div>
                <label className="block text-slate-700 mb-2">湿度 (%)</label>
                <input
                  type="text"
                  value={resultData.humidity}
                  onChange={(e) => setResultData({ ...resultData, humidity: e.target.value })}
                  className="w-full px-3 py-2 border border-slate-300 rounded focus:ring-2 focus:ring-blue-500 outline-none"
                />
              </div>
            </div>

            {/* Measurements Table */}
            <div>
              <label className="block text-slate-700 mb-3">测量数据表</label>
              <div className="overflow-x-auto border border-slate-300 rounded-lg">
                <table className="w-full">
                  <thead>
                    <tr className="bg-slate-100">
                      <th className="border border-slate-300 px-4 py-2">相位</th>
                      <th className="border border-slate-300 px-4 py-2">f(%)</th>
                      <th className="border border-slate-300 px-4 py-2">d(分)</th>
                      <th className="border border-slate-300 px-4 py-2">dU(%)</th>
                      <th className="border border-slate-300 px-4 py-2">Upt:U</th>
                      <th className="border border-slate-300 px-4 py-2">Uyb:U</th>
                    </tr>
                  </thead>
                  <tbody>
                    {(['ao', 'bo', 'co'] as const).map(phase => (
                      <tr key={phase}>
                        <td className="border border-slate-300 px-4 py-2">{phase}</td>
                        <td className="border border-slate-300 px-2 py-2">
                          <input
                            type="text"
                            value={resultData.measurements[phase].f}
                            onChange={(e) => updateMeasurement(phase, 'f', e.target.value)}
                            className="w-full px-2 py-1 border border-slate-300 rounded focus:ring-1 focus:ring-blue-500 outline-none"
                          />
                        </td>
                        <td className="border border-slate-300 px-2 py-2">
                          <input
                            type="text"
                            value={resultData.measurements[phase].d}
                            onChange={(e) => updateMeasurement(phase, 'd', e.target.value)}
                            className="w-full px-2 py-1 border border-slate-300 rounded focus:ring-1 focus:ring-blue-500 outline-none"
                          />
                        </td>
                        <td className="border border-slate-300 px-2 py-2">
                          <input
                            type="text"
                            value={resultData.measurements[phase].dU}
                            onChange={(e) => updateMeasurement(phase, 'dU', e.target.value)}
                            className="w-full px-2 py-1 border border-slate-300 rounded focus:ring-1 focus:ring-blue-500 outline-none"
                          />
                        </td>
                        <td className="border border-slate-300 px-2 py-2">
                          <input
                            type="text"
                            value={resultData.measurements[phase].Upt}
                            onChange={(e) => updateMeasurement(phase, 'Upt', e.target.value)}
                            className="w-full px-2 py-1 border border-slate-300 rounded focus:ring-1 focus:ring-blue-500 outline-none"
                          />
                        </td>
                        <td className="border border-slate-300 px-2 py-2">
                          <input
                            type="text"
                            value={resultData.measurements[phase].Uyb}
                            onChange={(e) => updateMeasurement(phase, 'Uyb', e.target.value)}
                            className="w-full px-2 py-1 border border-slate-300 rounded focus:ring-1 focus:ring-blue-500 outline-none"
                          />
                        </td>
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
              <div className="mt-2 text-right text-slate-600">
                <span>PT侧不带自校 / 按1测量 按2储存 / r% {resultData.rValue}</span>
              </div>
            </div>
          </div>

          <div className="flex justify-between gap-4 pt-6 border-t border-slate-200 mt-6">
            <button
              type="button"
              onClick={onBack}
              className="flex items-center gap-2 px-6 py-3 text-slate-700 border-2 border-slate-300 rounded-lg hover:bg-slate-50 transition"
            >
              <ArrowLeft className="w-5 h-5" />
              返回
            </button>
            <button
              type="submit"
              className="flex items-center gap-2 px-6 py-3 bg-blue-600 text-white rounded-lg hover:bg-blue-700 transition shadow-md hover:shadow-lg"
            >
              保存结果
              <ArrowRight className="w-5 h-5" />
            </button>
          </div>
        </form>
      )}
    </div>
  );
}
