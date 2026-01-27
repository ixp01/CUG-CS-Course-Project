import { useState } from 'react';
import { Save, ArrowRight, ArrowLeft, Plus, Trash2 } from 'lucide-react';

interface ExperimentRow {
  id: string;
  item: string;
  gear: string;
  percentage: string;
  lowerLimit: string;
  upperLimit: string;
  measuredValue: string;
}

interface ExperimentDataInputProps {
  data?: { rows: ExperimentRow[] };
  onSave: (data: { rows: ExperimentRow[] }) => void;
  onBack: () => void;
}

const defaultRows: ExperimentRow[] = [
  { id: '1', item: 'PT1', gear: '100V', percentage: '20%', lowerLimit: '19.6%', upperLimit: '20.4%', measuredValue: '' },
  { id: '2', item: 'PT2', gear: '100V', percentage: '100%', lowerLimit: '98.0%', upperLimit: '102.0%', measuredValue: '' },
  { id: '3', item: 'CT1', gear: '5A', percentage: '5%', lowerLimit: '4.90%', upperLimit: '5.1%', measuredValue: '' },
  { id: '4', item: 'CT2', gear: '5A', percentage: '100%', lowerLimit: '98.0%', upperLimit: '102.0%', measuredValue: '' },
];

export function ExperimentDataInput({ data, onSave, onBack }: ExperimentDataInputProps) {
  const [rows, setRows] = useState<ExperimentRow[]>(data?.rows || defaultRows);

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    onSave({ rows });
  };

  const updateRow = (id: string, field: keyof ExperimentRow, value: string) => {
    setRows(prev => prev.map(row => 
      row.id === id ? { ...row, [field]: value } : row
    ));
  };

  const addRow = () => {
    const newRow: ExperimentRow = {
      id: Date.now().toString(),
      item: '',
      gear: '',
      percentage: '',
      lowerLimit: '',
      upperLimit: '',
      measuredValue: ''
    };
    setRows([...rows, newRow]);
  };

  const deleteRow = (id: string) => {
    if (rows.length > 1) {
      setRows(prev => prev.filter(row => row.id !== id));
    }
  };

  return (
    <div className="bg-white rounded-lg shadow-md p-8">
      <div className="mb-6">
        <h2 className="text-slate-900">检测实验输入信息描述</h2>
        <p className="text-slate-600 mt-1">请输入检测实验的详细数据信息</p>
      </div>

      <form onSubmit={handleSubmit} className="space-y-6">
        <div className="overflow-x-auto">
          <table className="w-full border-collapse">
            <thead>
              <tr className="bg-blue-600 text-white">
                <th className="border border-blue-700 px-4 py-3">项目</th>
                <th className="border border-blue-700 px-4 py-3">档位</th>
                <th className="border border-blue-700 px-4 py-3">百分比</th>
                <th className="border border-blue-700 px-4 py-3">数据下限</th>
                <th className="border border-blue-700 px-4 py-3">数据上限</th>
                <th className="border border-blue-700 px-4 py-3">实测数据</th>
                <th className="border border-blue-700 px-4 py-3 w-20">操作</th>
              </tr>
            </thead>
            <tbody>
              {rows.map((row) => (
                <tr key={row.id} className="hover:bg-slate-50">
                  <td className="border border-slate-300 px-2 py-2">
                    <input
                      type="text"
                      value={row.item}
                      onChange={(e) => updateRow(row.id, 'item', e.target.value)}
                      className="w-full px-3 py-2 border border-slate-300 rounded focus:ring-2 focus:ring-blue-500 focus:border-blue-500 outline-none"
                      required
                    />
                  </td>
                  <td className="border border-slate-300 px-2 py-2">
                    <input
                      type="text"
                      value={row.gear}
                      onChange={(e) => updateRow(row.id, 'gear', e.target.value)}
                      className="w-full px-3 py-2 border border-slate-300 rounded focus:ring-2 focus:ring-blue-500 focus:border-blue-500 outline-none"
                      required
                    />
                  </td>
                  <td className="border border-slate-300 px-2 py-2">
                    <input
                      type="text"
                      value={row.percentage}
                      onChange={(e) => updateRow(row.id, 'percentage', e.target.value)}
                      className="w-full px-3 py-2 border border-slate-300 rounded focus:ring-2 focus:ring-blue-500 focus:border-blue-500 outline-none"
                      required
                    />
                  </td>
                  <td className="border border-slate-300 px-2 py-2">
                    <input
                      type="text"
                      value={row.lowerLimit}
                      onChange={(e) => updateRow(row.id, 'lowerLimit', e.target.value)}
                      className="w-full px-3 py-2 border border-slate-300 rounded focus:ring-2 focus:ring-blue-500 focus:border-blue-500 outline-none"
                      required
                    />
                  </td>
                  <td className="border border-slate-300 px-2 py-2">
                    <input
                      type="text"
                      value={row.upperLimit}
                      onChange={(e) => updateRow(row.id, 'upperLimit', e.target.value)}
                      className="w-full px-3 py-2 border border-slate-300 rounded focus:ring-2 focus:ring-blue-500 focus:border-blue-500 outline-none"
                      required
                    />
                  </td>
                  <td className="border border-slate-300 px-2 py-2">
                    <input
                      type="text"
                      value={row.measuredValue}
                      onChange={(e) => updateRow(row.id, 'measuredValue', e.target.value)}
                      className="w-full px-3 py-2 border border-slate-300 rounded focus:ring-2 focus:ring-blue-500 focus:border-blue-500 outline-none bg-blue-50"
                      placeholder="待测量"
                    />
                  </td>
                  <td className="border border-slate-300 px-2 py-2 text-center">
                    <button
                      type="button"
                      onClick={() => deleteRow(row.id)}
                      className="p-2 text-red-600 hover:bg-red-50 rounded transition"
                      disabled={rows.length <= 1}
                    >
                      <Trash2 className="w-4 h-4" />
                    </button>
                  </td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>

        <button
          type="button"
          onClick={addRow}
          className="flex items-center gap-2 px-4 py-2 text-blue-600 border-2 border-blue-600 rounded-lg hover:bg-blue-50 transition"
        >
          <Plus className="w-5 h-5" />
          添加实验项目
        </button>

        <div className="bg-yellow-50 border border-yellow-200 rounded-lg p-4">
          <p className="text-yellow-800">
            <span>注意：</span> 每次实验可选择PT1、PT2、CT1和CT2一种项目或多个项目进行实验
          </p>
        </div>

        <div className="flex justify-between gap-4 pt-6 border-t border-slate-200">
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
            <Save className="w-5 h-5" />
            保存并继续
            <ArrowRight className="w-5 h-5" />
          </button>
        </div>
      </form>
    </div>
  );
}
