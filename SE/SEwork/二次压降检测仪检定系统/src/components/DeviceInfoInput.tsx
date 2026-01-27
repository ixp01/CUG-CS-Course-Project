import { useState } from 'react';
import { Save, ArrowRight } from 'lucide-react';

interface DeviceInfo {
  productCode: string;
  productName: string;
  manufacturer: string;
  productionDate: string;
  inspectionDate: string;
  origin: string;
  modelNumber: string;
  notes: string;
}

interface DeviceInfoInputProps {
  data?: DeviceInfo;
  onSave: (data: DeviceInfo) => void;
}

export function DeviceInfoInput({ data, onSave }: DeviceInfoInputProps) {
  const [formData, setFormData] = useState<DeviceInfo>(data || {
    productCode: '',
    productName: '互感器二次压降检测仪',
    manufacturer: '',
    productionDate: '',
    inspectionDate: new Date().toISOString().split('T')[0],
    origin: '',
    modelNumber: '',
    notes: ''
  });

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    onSave(formData);
  };

  const handleChange = (field: keyof DeviceInfo, value: string) => {
    setFormData(prev => ({ ...prev, [field]: value }));
  };

  return (
    <div className="bg-white rounded-lg shadow-md p-8">
      <div className="mb-6">
        <h2 className="text-slate-900">设备信息录入</h2>
        <p className="text-slate-600 mt-1">请填写被检测设备的基本信息</p>
      </div>

      <form onSubmit={handleSubmit} className="space-y-6">
        <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
          <div>
            <label className="block text-slate-700 mb-2">
              产品编号 <span className="text-red-500">*</span>
            </label>
            <input
              type="text"
              required
              value={formData.productCode}
              onChange={(e) => handleChange('productCode', e.target.value)}
              className="w-full px-4 py-2 border border-slate-300 rounded-lg focus:ring-2 focus:ring-blue-500 focus:border-blue-500 outline-none transition"
              placeholder="请输入产品编号"
            />
          </div>

          <div>
            <label className="block text-slate-700 mb-2">
              产品名称 <span className="text-red-500">*</span>
            </label>
            <input
              type="text"
              required
              value={formData.productName}
              onChange={(e) => handleChange('productName', e.target.value)}
              className="w-full px-4 py-2 border border-slate-300 rounded-lg focus:ring-2 focus:ring-blue-500 focus:border-blue-500 outline-none transition"
              placeholder="请输入产品名称"
            />
          </div>

          <div>
            <label className="block text-slate-700 mb-2">
              制造商 <span className="text-red-500">*</span>
            </label>
            <input
              type="text"
              required
              value={formData.manufacturer}
              onChange={(e) => handleChange('manufacturer', e.target.value)}
              className="w-full px-4 py-2 border border-slate-300 rounded-lg focus:ring-2 focus:ring-blue-500 focus:border-blue-500 outline-none transition"
              placeholder="请输入制造商"
            />
          </div>

          <div>
            <label className="block text-slate-700 mb-2">
              生产日期 <span className="text-red-500">*</span>
            </label>
            <input
              type="date"
              required
              value={formData.productionDate}
              onChange={(e) => handleChange('productionDate', e.target.value)}
              className="w-full px-4 py-2 border border-slate-300 rounded-lg focus:ring-2 focus:ring-blue-500 focus:border-blue-500 outline-none transition"
            />
          </div>

          <div>
            <label className="block text-slate-700 mb-2">
              送检日期 <span className="text-red-500">*</span>
            </label>
            <input
              type="date"
              required
              value={formData.inspectionDate}
              onChange={(e) => handleChange('inspectionDate', e.target.value)}
              className="w-full px-4 py-2 border border-slate-300 rounded-lg focus:ring-2 focus:ring-blue-500 focus:border-blue-500 outline-none transition"
            />
          </div>

          <div>
            <label className="block text-slate-700 mb-2">
              产地
            </label>
            <input
              type="text"
              value={formData.origin}
              onChange={(e) => handleChange('origin', e.target.value)}
              className="w-full px-4 py-2 border border-slate-300 rounded-lg focus:ring-2 focus:ring-blue-500 focus:border-blue-500 outline-none transition"
              placeholder="请输入产地"
            />
          </div>

          <div>
            <label className="block text-slate-700 mb-2">
              产品型号
            </label>
            <input
              type="text"
              value={formData.modelNumber}
              onChange={(e) => handleChange('modelNumber', e.target.value)}
              className="w-full px-4 py-2 border border-slate-300 rounded-lg focus:ring-2 focus:ring-blue-500 focus:border-blue-500 outline-none transition"
              placeholder="请输入产品型号"
            />
          </div>

          <div>
            <label className="block text-slate-700 mb-2">
              备注
            </label>
            <input
              type="text"
              value={formData.notes}
              onChange={(e) => handleChange('notes', e.target.value)}
              className="w-full px-4 py-2 border border-slate-300 rounded-lg focus:ring-2 focus:ring-blue-500 focus:border-blue-500 outline-none transition"
              placeholder="其他说明信息"
            />
          </div>
        </div>

        <div className="flex justify-end gap-4 pt-6 border-t border-slate-200">
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
