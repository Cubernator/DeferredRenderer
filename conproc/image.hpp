#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "processor.hpp"

class image_processor : public processor
{
public:
	image_processor(const conproc* parent);

protected:
	virtual void process_impl(const fs::path& file, const nlohmann::json& options) override;

private:
	std::vector<unsigned char> m_inputData, m_outputData;

	unsigned int m_width, m_height;
	bool m_compressed;
	int m_comprFlags;

	bool m_inputSuccess, m_convertSuccess, m_outputSuccess;

	void importTIFF(const fs::path& file);
	void importSTB(const fs::path& file);

	void parseFormat(const std::string& fmtStr, bool& isCompressed, int& comprFlags);

	void convertCompressed(int flags);
	void convertUncompressed();

	void writeFile(const fs::path& file, const nlohmann::json& options);
};

#endif // IMAGE_HPP