// YAL zeldan

#ifndef CXU_TEXT_HXX
#define CXU_TEXT_HXX

#include <string>

#include "defs.hxx"
#include "types.hxx"
#include "memory.hxx"
#include "file.hxx"
#include "integer.hxx"

CXU_NS_BEGIN

//! abstract text output
class TextOutput : Entity
{
public:
    virtual ~TextOutput() = default;
    virtual void write(char) = 0;
    virtual void write(const char *) = 0;
    virtual void writeln() = 0;
    virtual void writeln(const char *) = 0;

    void write(const std::string & str)
    {
        write(str.data());
    }

    void writeln(const std::string & str)
    {
        writeln(str.data());
    }

    //! hexa and char dump of segment to text
    void dump(const ConstSegment &, const std::string & prefix);


};

//! abstract text input

class TextInput : Entity
{
public:
    virtual ~TextInput() = default;

    explicit TextInput(char q = 127) : _q(q)
    {
    }

    virtual bool read(std::string &) = 0;
    virtual std::string read() = 0;

    char validate(char k);

private:
    char _q;
};

//! abstract text output to file

class TextOutputFile : virtual public TextOutput
{
public:
    explicit TextOutputFile(WritableFile & s);

    using TextOutput::write;
    using TextOutput::writeln;

    virtual void write(char) override;
    virtual void write(const char *) override;
    virtual void writeln() override;
    virtual void writeln(const char *) override;


protected:

    WritableFile & _s;

};

//! abstract text input from file

class TextInputFile : virtual public TextInput
{
public:
    explicit TextInputFile(ReadableFile & s);

    virtual bool read(std::string &) override;
    virtual std::string read() override;


protected:

    ReadableFile & _s;

};




//! TextOutput to ANSI-C FILE

class TextOutputFILE : public TextOutput
{
public:

    explicit TextOutputFILE(FILE * f) : _f(f)
    {
    }

    explicit TextOutputFILE(const AnsiFILE & f) : _f(f.file())
    {
    }

    virtual void write(char) override;
    virtual void write(const char *) override;
    virtual void writeln() override;
    virtual void writeln(const char *) override;


    using TextOutput::write;
    using TextOutput::writeln;

    void flush();

protected:
    FILE * _f;
};


//! TextInput from ANSI-C FILE

class TextInputFILE : public TextInput
{
public:

    explicit TextInputFILE(FILE * f) : _f(f)
    {
    }

    explicit TextInputFILE(const AnsiFILE & f) : _f(f.file())
    {
    }

    virtual bool read(std::string &) override;
    virtual std::string read() override;



protected:
    FILE * _f;
};



//! TextOutput to Buffer

class TextOutputBuffer : public TextOutput
{
public:

    explicit TextOutputBuffer(Buffer & b) : _buffer(b)
    {
    }

    virtual void write(char) override;
    virtual void write(const char *) override;
    virtual void writeln() override;
    virtual void writeln(const char *) override;

    using TextOutput::write;
    using TextOutput::writeln;


private:
    Buffer & _buffer;
};

class TextInputConstSegment : public TextInput
{
public:

    explicit TextInputConstSegment(ConstSegment s) : _cseg(s), _ptr(0)
    {
    }

    virtual bool read(std::string &) override;
    virtual std::string read() override;


private:
    const ConstSegment _cseg;
    size_t _ptr;

    int readChar();
};


//! Properties management

class Properties
{
public:
    virtual ~Properties() = default;

    bool has(const std::string & key) const;

    const std::string & get(const std::string &) const;
    void set(const std::string & key, const std::string & val);
    bool mayset(const std::string & key, const std::string & val); // only if not already set

    const std::string & operator()(const std::string &) const;
    std::string & operator[](const std::string &);

    const std::string & getString(const std::string & key, const std::string & def) const;
    int getInt(const std::string & key, int def) const;
    unsigned getUnsigned(const std::string & key, unsigned def) const;

    void clear();
    bool any() const;
    void del(const std::string & p);
    bool trydel(const std::string & p); // no error if not found

    typedef std::map<std::string, std::string> t_map;

    const t_map & operator+() const
    {
        return _map;
    }

protected:

    t_map & operator+()
    {
        return _map;
    }


private:

    t_map _map;

};


void loadProperties_bl(Properties &, TextInput &); // add to existing
void saveProperties_bl(const Properties &, TextOutput &); // add to existing
void loadProperties_eq(Properties &, TextInput &); // add to existing
void saveProperties_eq(const Properties &, TextOutput &); // add to existing

class TextPackerOutput : Entity
{
public:

    enum class t_eol
    {
        e_cr, e_lf, e_crlf
    };
    virtual ~TextPackerOutput();
    TextPackerOutput(WritableFile &, t_eol);

    void push(const std::string &);
    void push(const char *);
    void force(const std::string &);
    void force(const char *);

    bool write();

    const char * s_eol(t_eol);

    bool empty() const
    {
        return _buffer.empty() && _outstr.empty();
    }

    bool avail() const
    {
        return _outstr.empty();
    }

private:
    WritableFile & _file;
    std::string _eol;
    std::string _outstr;
    Circular _buffer;

    void str2buf();
};

class TextPackerInput : Entity
{
public:
    virtual ~TextPackerInput();
    TextPackerInput(ReadableFile &, char inval);

    bool read(); // return false if File closed
    bool pull(std::string &); //return true if pulled

    bool isvalid(const std::string & s)
    {
        return s.find(_inval) == std::string::npos;
    }

    bool empty() const
    {
        return _buffer.empty() && _build.empty();
    }

    void clear();

private:
    ReadableFile & _file;
    char _inval;
    Circular _buffer;
    std::string _build;
};



CXU_NS_END

#endif


//.
